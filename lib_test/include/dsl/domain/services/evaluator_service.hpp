#pragma once

#include "../model/ast_nodes.hpp"
#include "../ports/outbound/context_repository_port.hpp"
#include <stack>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <vector>

namespace dsl::domain::services {

class ExpressionEvaluatorService final : public model::IAstVisitor {
public:
    explicit ExpressionEvaluatorService(const ports::outbound::IContextRepositoryPort& context)
        : context_(context) {}

    model::Literal evaluate(const model::IAstNode& node) {
        node.accept(*this);
        if (eval_stack_.empty()) {
            throw std::runtime_error("Evaluation stack empty after evaluation");
        }
        auto result = eval_stack_.top();
        eval_stack_.pop();
        return result;
    }

    void visit(const model::LiteralNode& node) override {
        eval_stack_.push(node.value());
    }

    void visit(const model::VariableNode& node) override {
        const auto& var_name = node.identifier().value();

        // Built-in Mathematical Constants
        if (var_name == "pi" || var_name == "PI") {
            eval_stack_.push(model::Literal(3.14159265358979323846));
            return;
        }
        if (var_name == "e" || var_name == "E") {
            eval_stack_.push(model::Literal(2.71828182845904523536));
            return;
        }
        if (var_name == "tau" || var_name == "TAU") {
            eval_stack_.push(model::Literal(6.28318530717958647692));
            return;
        }

        auto val_opt = context_.get_variable(var_name);
        if (!val_opt.has_value()) {
            throw std::runtime_error("Undefined variable in context: '" + var_name + "'");
        }
        eval_stack_.push(*val_opt);
    }

    void visit(const model::BinaryNode& node) override {
        node.left()->accept(*this);
        auto left = eval_stack_.top();
        eval_stack_.pop();

        node.right()->accept(*this);
        auto right = eval_stack_.top();
        eval_stack_.pop();

        eval_stack_.push(eval_binary(node.op(), left, right));
    }

    void visit(const model::UnaryNode& node) override {
        node.operand()->accept(*this);
        auto operand = eval_stack_.top();
        eval_stack_.pop();

        eval_stack_.push(eval_unary(node.op(), operand));
    }

    void visit(const model::FunctionCallNode& node) override {
        std::vector<model::Literal> evaluated_args;
        for (const auto& arg : node.args()) {
            arg->accept(*this);
            evaluated_args.push_back(eval_stack_.top());
            eval_stack_.pop();
        }
        eval_stack_.push(eval_function(node.name(), evaluated_args));
    }

private:
    model::Literal eval_binary(model::OperatorType op, const model::Literal& left, const model::Literal& right) {
        using model::OperatorType;

        // String concatenation
        if (op == OperatorType::Add && (left.is_string() || right.is_string())) {
            std::string l_str = left.is_string() ? left.as_string() : left.to_string_repr();
            std::string r_str = right.is_string() ? right.as_string() : right.to_string_repr();
            return model::Literal(l_str + r_str);
        }

        // Equality operations
        if (op == OperatorType::Equal) {
            return model::Literal(left == right);
        }
        if (op == OperatorType::NotEqual) {
            return model::Literal(left != right);
        }

        // Logical operations (And, Or)
        if (op == OperatorType::And) {
            return model::Literal(left.as_bool() && right.as_bool());
        }
        if (op == OperatorType::Or) {
            return model::Literal(left.as_bool() || right.as_bool());
        }

        // Integer Bitwise Operations
        if (op == OperatorType::BitAnd || op == OperatorType::BitOr || 
            op == OperatorType::BitXor || op == OperatorType::ShiftLeft || op == OperatorType::ShiftRight) {
            if (!left.is_int() || !right.is_int()) {
                throw std::runtime_error("Bitwise operations require integer operands");
            }
            int64_t l = left.as_int();
            int64_t r = right.as_int();
            switch (op) {
                case OperatorType::BitAnd:     return model::Literal(l & r);
                case OperatorType::BitOr:      return model::Literal(l | r);
                case OperatorType::BitXor:     return model::Literal(l ^ r);
                case OperatorType::ShiftLeft:  return model::Literal(l << r);
                case OperatorType::ShiftRight: return model::Literal(l >> r);
                default: break;
            }
        }

        // Numeric power
        if (op == OperatorType::Power) {
            if (!left.is_number() || !right.is_number()) {
                throw std::runtime_error("Power operator '^' requires numeric operands");
            }
            if (left.is_int() && right.is_int() && right.as_int() >= 0) {
                int64_t base = left.as_int();
                int64_t exp = right.as_int();
                int64_t res = 1;
                while (exp > 0) {
                    if (exp & 1) res *= base;
                    base *= base;
                    exp >>= 1;
                }
                return model::Literal(res);
            }
            return model::Literal(std::pow(left.to_number(), right.to_number()));
        }

        // Numeric comparisons and arithmetic
        if (left.is_number() && right.is_number()) {
            bool use_float = left.is_float() || right.is_float();

            if (use_float) {
                double l = left.to_number();
                double r = right.to_number();
                switch (op) {
                    case OperatorType::Add:          return model::Literal(l + r);
                    case OperatorType::Subtract:     return model::Literal(l - r);
                    case OperatorType::Multiply:     return model::Literal(l * r);
                    case OperatorType::Divide: {
                        if (std::abs(r) < 1e-15) throw std::runtime_error("Division by zero");
                        return model::Literal(l / r);
                    }
                    case OperatorType::Less:         return model::Literal(l < r);
                    case OperatorType::LessEqual:    return model::Literal(l <= r);
                    case OperatorType::Greater:      return model::Literal(l > r);
                    case OperatorType::GreaterEqual: return model::Literal(l >= r);
                    default: break;
                }
            } else {
                int64_t l = left.as_int();
                int64_t r = right.as_int();
                switch (op) {
                    case OperatorType::Add:          return model::Literal(l + r);
                    case OperatorType::Subtract:     return model::Literal(l - r);
                    case OperatorType::Multiply:     return model::Literal(l * r);
                    case OperatorType::Divide: {
                        if (r == 0) throw std::runtime_error("Division by zero");
                        return model::Literal(l / r);
                    }
                    case OperatorType::Modulo: {
                        if (r == 0) throw std::runtime_error("Modulo by zero");
                        return model::Literal(l % r);
                    }
                    case OperatorType::Less:         return model::Literal(l < r);
                    case OperatorType::LessEqual:    return model::Literal(l <= r);
                    case OperatorType::Greater:      return model::Literal(l > r);
                    case OperatorType::GreaterEqual: return model::Literal(l >= r);
                    default: break;
                }
            }
        }

        // String ordering
        if (left.is_string() && right.is_string()) {
            switch (op) {
                case OperatorType::Less:         return model::Literal(left.as_string() < right.as_string());
                case OperatorType::LessEqual:    return model::Literal(left.as_string() <= right.as_string());
                case OperatorType::Greater:      return model::Literal(left.as_string() > right.as_string());
                case OperatorType::GreaterEqual: return model::Literal(left.as_string() >= right.as_string());
                default: break;
            }
        }

        throw std::runtime_error("Unsupported operand types for operator " + model::to_string(op));
    }

    model::Literal eval_unary(model::OperatorType op, const model::Literal& operand) {
        using model::OperatorType;
        if (op == OperatorType::Not) {
            return model::Literal(!operand.as_bool());
        }
        if (op == OperatorType::Subtract) {
            if (operand.is_int()) return model::Literal(-operand.as_int());
            if (operand.is_float()) return model::Literal(-operand.as_float());
            throw std::runtime_error("Unary '-' requires a numeric operand");
        }
        if (op == OperatorType::BitNot) {
            if (!operand.is_int()) throw std::runtime_error("Unary '~' requires an integer operand");
            return model::Literal(~operand.as_int());
        }
        throw std::runtime_error("Unsupported unary operator " + model::to_string(op));
    }

    model::Literal eval_function(const std::string& name, const std::vector<model::Literal>& args) {
        auto lower_name = name;
        std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), [](unsigned char c){ return std::tolower(c); });

        // Helper check
        auto require_args = [&](size_t count) {
            if (args.size() != count) {
                throw std::runtime_error("Function '" + name + "' requires " + std::to_string(count) + " arguments");
            }
        };

        // Single-argument math functions
        if (lower_name == "abs") {
            require_args(1);
            if (args[0].is_int()) return model::Literal(std::abs(args[0].as_int()));
            return model::Literal(std::abs(args[0].to_number()));
        }
        if (lower_name == "sign" || lower_name == "sgn") {
            require_args(1);
            double v = args[0].to_number();
            return model::Literal(v > 0.0 ? 1 : (v < 0.0 ? -1 : 0));
        }
        if (lower_name == "sqrt") {
            require_args(1);
            double v = args[0].to_number();
            if (v < 0.0) throw std::runtime_error("sqrt: domain error (negative input)");
            return model::Literal(std::sqrt(v));
        }
        if (lower_name == "cbrt") {
            require_args(1);
            return model::Literal(std::cbrt(args[0].to_number()));
        }
        if (lower_name == "exp") {
            require_args(1);
            return model::Literal(std::exp(args[0].to_number()));
        }
        if (lower_name == "exp2") {
            require_args(1);
            return model::Literal(std::exp2(args[0].to_number()));
        }
        if (lower_name == "log" || lower_name == "ln") {
            require_args(1);
            double v = args[0].to_number();
            if (v <= 0.0) throw std::runtime_error("log: domain error (non-positive input)");
            return model::Literal(std::log(v));
        }
        if (lower_name == "log10") {
            require_args(1);
            double v = args[0].to_number();
            if (v <= 0.0) throw std::runtime_error("log10: domain error (non-positive input)");
            return model::Literal(std::log10(v));
        }
        if (lower_name == "log2") {
            require_args(1);
            double v = args[0].to_number();
            if (v <= 0.0) throw std::runtime_error("log2: domain error (non-positive input)");
            return model::Literal(std::log2(v));
        }
        if (lower_name == "sin") {
            require_args(1);
            return model::Literal(std::sin(args[0].to_number()));
        }
        if (lower_name == "cos") {
            require_args(1);
            return model::Literal(std::cos(args[0].to_number()));
        }
        if (lower_name == "tan") {
            require_args(1);
            return model::Literal(std::tan(args[0].to_number()));
        }
        if (lower_name == "asin") {
            require_args(1);
            return model::Literal(std::asin(args[0].to_number()));
        }
        if (lower_name == "acos") {
            require_args(1);
            return model::Literal(std::acos(args[0].to_number()));
        }
        if (lower_name == "atan") {
            require_args(1);
            return model::Literal(std::atan(args[0].to_number()));
        }
        if (lower_name == "sinh") {
            require_args(1);
            return model::Literal(std::sinh(args[0].to_number()));
        }
        if (lower_name == "cosh") {
            require_args(1);
            return model::Literal(std::cosh(args[0].to_number()));
        }
        if (lower_name == "tanh") {
            require_args(1);
            return model::Literal(std::tanh(args[0].to_number()));
        }
        if (lower_name == "asinh") {
            require_args(1);
            return model::Literal(std::asinh(args[0].to_number()));
        }
        if (lower_name == "acosh") {
            require_args(1);
            return model::Literal(std::acosh(args[0].to_number()));
        }
        if (lower_name == "atanh") {
            require_args(1);
            return model::Literal(std::atanh(args[0].to_number()));
        }
        if (lower_name == "floor") {
            require_args(1);
            if (args[0].is_int()) return args[0];
            return model::Literal(std::floor(args[0].to_number()));
        }
        if (lower_name == "ceil") {
            require_args(1);
            if (args[0].is_int()) return args[0];
            return model::Literal(std::ceil(args[0].to_number()));
        }
        if (lower_name == "round") {
            require_args(1);
            if (args[0].is_int()) return args[0];
            return model::Literal(std::round(args[0].to_number()));
        }
        if (lower_name == "trunc") {
            require_args(1);
            if (args[0].is_int()) return args[0];
            return model::Literal(std::trunc(args[0].to_number()));
        }
        if (lower_name == "erf") {
            require_args(1);
            return model::Literal(std::erf(args[0].to_number()));
        }
        if (lower_name == "gamma" || lower_name == "tgamma") {
            require_args(1);
            return model::Literal(std::tgamma(args[0].to_number()));
        }
        if (lower_name == "factorial") {
            require_args(1);
            int64_t n = args[0].as_int();
            if (n < 0 || n > 20) throw std::runtime_error("factorial: n must be between 0 and 20");
            int64_t res = 1;
            for (int64_t i = 2; i <= n; ++i) res *= i;
            return model::Literal(res);
        }

        // Two-argument math functions
        if (lower_name == "pow") {
            require_args(2);
            return eval_binary(model::OperatorType::Power, args[0], args[1]);
        }
        if (lower_name == "atan2") {
            require_args(2);
            return model::Literal(std::atan2(args[0].to_number(), args[1].to_number()));
        }
        if (lower_name == "hypot") {
            require_args(2);
            return model::Literal(std::hypot(args[0].to_number(), args[1].to_number()));
        }
        if (lower_name == "gcd") {
            require_args(2);
            return model::Literal(std::gcd(args[0].as_int(), args[1].as_int()));
        }
        if (lower_name == "lcm") {
            require_args(2);
            return model::Literal(std::lcm(args[0].as_int(), args[1].as_int()));
        }
        if (lower_name == "min") {
            if (args.empty()) throw std::runtime_error("min requires at least 1 argument");
            auto min_val = args[0];
            for (size_t i = 1; i < args.size(); ++i) {
                if (args[i].to_number() < min_val.to_number()) {
                    min_val = args[i];
                }
            }
            return min_val;
        }
        if (lower_name == "max") {
            if (args.empty()) throw std::runtime_error("max requires at least 1 argument");
            auto max_val = args[0];
            for (size_t i = 1; i < args.size(); ++i) {
                if (args[i].to_number() > max_val.to_number()) {
                    max_val = args[i];
                }
            }
            return max_val;
        }

        // Three-argument math functions
        if (lower_name == "clamp") {
            require_args(3);
            double v = args[0].to_number();
            double lo = args[1].to_number();
            double hi = args[2].to_number();
            if (lo > hi) throw std::runtime_error("clamp: lo must be <= hi");
            return model::Literal(std::clamp(v, lo, hi));
        }

        throw std::runtime_error("Unknown math function in DSL: '" + name + "'");
    }

    const ports::outbound::IContextRepositoryPort& context_;
    std::stack<model::Literal> eval_stack_;
};

} // namespace dsl::domain::services
