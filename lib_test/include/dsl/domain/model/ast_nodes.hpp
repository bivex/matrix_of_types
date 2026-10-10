#pragma once

#include "types.hpp"
#include <memory>
#include <string>

namespace dsl::domain::model {

class LiteralNode;
class VariableNode;
class BinaryNode;
class UnaryNode;

// Visitor Interface for AST
class IAstVisitor {
public:
    virtual ~IAstVisitor() = default;
    virtual void visit(const LiteralNode& node) = 0;
    virtual void visit(const VariableNode& node) = 0;
    virtual void visit(const BinaryNode& node) = 0;
    virtual void visit(const UnaryNode& node) = 0;
};

// Base AST Node (Immutable)
class IAstNode {
public:
    virtual ~IAstNode() = default;
    [[nodiscard]] virtual std::string to_string() const = 0;
    virtual void accept(IAstVisitor& visitor) const = 0;
};

using AstNodePtr = std::shared_ptr<const IAstNode>;

// Leaf Node: Literal value
class LiteralNode final : public IAstNode {
public:
    explicit LiteralNode(Literal value) : value_(std::move(value)) {}

    [[nodiscard]] const Literal& value() const noexcept { return value_; }

    [[nodiscard]] std::string to_string() const override {
        return value_.to_string_repr();
    }

    void accept(IAstVisitor& visitor) const override {
        visitor.visit(*this);
    }

private:
    Literal value_;
};

// Leaf Node: Variable / Identifier reference
class VariableNode final : public IAstNode {
public:
    explicit VariableNode(Identifier identifier) : identifier_(std::move(identifier)) {}

    [[nodiscard]] const Identifier& identifier() const noexcept { return identifier_; }

    [[nodiscard]] std::string to_string() const override {
        return identifier_.value();
    }

    void accept(IAstVisitor& visitor) const override {
        visitor.visit(*this);
    }

private:
    Identifier identifier_;
};

// Internal Node: Binary operation
class BinaryNode final : public IAstNode {
public:
    BinaryNode(OperatorType op, AstNodePtr left, AstNodePtr right)
        : op_(op), left_(std::move(left)), right_(std::move(right)) {
        if (!left_ || !right_) {
            throw std::invalid_argument("BinaryNode child nodes cannot be null");
        }
    }

    [[nodiscard]] OperatorType op() const noexcept { return op_; }
    [[nodiscard]] const AstNodePtr& left() const noexcept { return left_; }
    [[nodiscard]] const AstNodePtr& right() const noexcept { return right_; }

    [[nodiscard]] std::string to_string() const override {
        return "(" + left_->to_string() + " " + model::to_string(op_) + " " + right_->to_string() + ")";
    }

    void accept(IAstVisitor& visitor) const override {
        visitor.visit(*this);
    }

private:
    OperatorType op_;
    AstNodePtr left_;
    AstNodePtr right_;
};

// Internal Node: Unary operation
class UnaryNode final : public IAstNode {
public:
    UnaryNode(OperatorType op, AstNodePtr operand)
        : op_(op), operand_(std::move(operand)) {
        if (!operand_) {
            throw std::invalid_argument("UnaryNode operand cannot be null");
        }
    }

    [[nodiscard]] OperatorType op() const noexcept { return op_; }
    [[nodiscard]] const AstNodePtr& operand() const noexcept { return operand_; }

    [[nodiscard]] std::string to_string() const override {
        return model::to_string(op_) + "(" + operand_->to_string() + ")";
    }

    void accept(IAstVisitor& visitor) const override {
        visitor.visit(*this);
    }

private:
    OperatorType op_;
    AstNodePtr operand_;
};

} // namespace dsl::domain::model
