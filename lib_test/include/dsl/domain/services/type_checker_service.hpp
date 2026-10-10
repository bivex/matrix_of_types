#pragma once

#include "../model/ast_nodes.hpp"
#include <optional>
#include <string>
#include <stack>
#include <algorithm>

namespace dsl::domain::services {

struct TypeCheckResult {
    bool is_valid{true};
    model::DslType inferred_type{model::DslType::Null};
    std::string error_message;

    static TypeCheckResult ok(model::DslType t) {
        return TypeCheckResult{true, t, ""};
    }

    static TypeCheckResult error(std::string msg) {
        return TypeCheckResult{false, model::DslType::Null, std::move(msg)};
    }
};

class TypeCheckerService final : public model::IAstVisitor {
public:
    TypeCheckResult validate(const model::IAstNode& node) {
        has_error_ = false;
        error_msg_.clear();
        type_stack_ = std::stack<model::DslType>{};

        node.accept(*this);

        if (has_error_) {
            return TypeCheckResult::error(error_msg_);
        }
        if (type_stack_.empty()) {
            return TypeCheckResult::error("Empty evaluation during type check");
        }
        return TypeCheckResult::ok(type_stack_.top());
    }

    void visit(const model::LiteralNode& node) override {
        type_stack_.push(node.value().type());
    }

    void visit(const model::VariableNode&) override {
        // Dynamic variable types are resolved at runtime
        type_stack_.push(model::DslType::Null);
    }

    void visit(const model::BinaryNode& node) override {
        node.left()->accept(*this);
        if (has_error_) return;
        auto left_t = type_stack_.top();
        type_stack_.pop();

        node.right()->accept(*this);
        if (has_error_) return;
        auto right_t = type_stack_.top();
        type_stack_.pop();

        using model::OperatorType;
        using model::DslType;

        // Logical
        if (node.op() == OperatorType::And || node.op() == OperatorType::Or) {
            if (left_t != DslType::Null && left_t != DslType::Boolean) {
                fail("Left operand of logical operator must be Boolean");
                return;
            }
            if (right_t != DslType::Null && right_t != DslType::Boolean) {
                fail("Right operand of logical operator must be Boolean");
                return;
            }
            type_stack_.push(DslType::Boolean);
            return;
        }

        // Relational
        if (node.op() == OperatorType::Less || node.op() == OperatorType::LessEqual ||
            node.op() == OperatorType::Greater || node.op() == OperatorType::GreaterEqual) {
            type_stack_.push(DslType::Boolean);
            return;
        }

        // Equality
        if (node.op() == OperatorType::Equal || node.op() == OperatorType::NotEqual) {
            type_stack_.push(DslType::Boolean);
            return;
        }

        // Bitwise
        if (node.op() == OperatorType::BitAnd || node.op() == OperatorType::BitOr ||
            node.op() == OperatorType::BitXor || node.op() == OperatorType::ShiftLeft ||
            node.op() == OperatorType::ShiftRight) {
            type_stack_.push(DslType::Integer);
            return;
        }

        // Arithmetic
        if (node.op() == OperatorType::Add) {
            if (left_t == DslType::String || right_t == DslType::String) {
                type_stack_.push(DslType::String);
                return;
            }
        }

        if (node.op() == OperatorType::Power) {
            type_stack_.push(DslType::Float);
            return;
        }

        if (left_t == DslType::Float || right_t == DslType::Float) {
            type_stack_.push(DslType::Float);
        } else {
            type_stack_.push(DslType::Integer);
        }
    }

    void visit(const model::UnaryNode& node) override {
        node.operand()->accept(*this);
        if (has_error_) return;
        auto op_t = type_stack_.top();
        type_stack_.pop();

        using model::OperatorType;
        using model::DslType;

        if (node.op() == OperatorType::Not) {
            if (op_t != DslType::Null && op_t != DslType::Boolean) {
                fail("Operand of '!' must be Boolean");
                return;
            }
            type_stack_.push(DslType::Boolean);
        } else if (node.op() == OperatorType::Subtract) {
            type_stack_.push(op_t == DslType::Float ? DslType::Float : DslType::Integer);
        } else if (node.op() == OperatorType::BitNot) {
            type_stack_.push(DslType::Integer);
        }
    }

    void visit(const model::FunctionCallNode& node) override {
        for (const auto& arg : node.args()) {
            arg->accept(*this);
            if (has_error_) return;
            type_stack_.pop();
        }

        auto lower_name = node.name();
        std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), [](unsigned char c){ return std::tolower(c); });

        if (lower_name == "gcd" || lower_name == "lcm" || lower_name == "factorial") {
            type_stack_.push(model::DslType::Integer);
        } else {
            type_stack_.push(model::DslType::Float);
        }
    }

private:
    void fail(std::string msg) {
        has_error_ = true;
        error_msg_ = std::move(msg);
    }

    bool has_error_{false};
    std::string error_msg_;
    std::stack<model::DslType> type_stack_;
};

} // namespace dsl::domain::services
