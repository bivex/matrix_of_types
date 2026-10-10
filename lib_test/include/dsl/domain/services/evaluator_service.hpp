#pragma once

#include "../model/ast_nodes.hpp"
#include "../ports/outbound/context_repository_port.hpp"
#include <stack>
#include <cmath>

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

private:
    model::Literal eval_binary(model::OperatorType op, const model::Literal& left, const model::Literal& right) {
        using model::OperatorType;

        // String operations
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
                        if (std::abs(r) < 1e-12) throw std::runtime_error("Division by zero");
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
        throw std::runtime_error("Unsupported unary operator " + model::to_string(op));
    }

    const ports::outbound::IContextRepositoryPort& context_;
    std::stack<model::Literal> eval_stack_;
};

} // namespace dsl::domain::services
