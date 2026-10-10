#pragma once

#include "../../domain/model/ast_nodes.hpp"
#include "../../domain/model/types.hpp"
#include <memory>
#include <string>
#include <concepts>

namespace dsl::adapters::inbound {

// Expression Wrapper for C++ Embedded DSL
class Expr {
public:
    using is_dsl_expr_tag = void;

    explicit Expr(domain::model::AstNodePtr node) : node_(std::move(node)) {}

    template<typename T>
        requires (!std::same_as<std::decay_t<T>, Expr> && 
                  (std::integral<std::decay_t<T>> || 
                   std::floating_point<std::decay_t<T>> || 
                   std::convertible_to<T, std::string> || 
                   std::same_as<std::decay_t<T>, bool>))
    explicit Expr(T&& val) {
        if constexpr (std::same_as<std::decay_t<T>, bool>) {
            node_ = std::make_shared<domain::model::LiteralNode>(domain::model::Literal(val));
        } else if constexpr (std::integral<std::decay_t<T>>) {
            node_ = std::make_shared<domain::model::LiteralNode>(domain::model::Literal(static_cast<int64_t>(val)));
        } else if constexpr (std::floating_point<std::decay_t<T>>) {
            node_ = std::make_shared<domain::model::LiteralNode>(domain::model::Literal(static_cast<double>(val)));
        } else {
            node_ = std::make_shared<domain::model::LiteralNode>(domain::model::Literal(std::string(val)));
        }
    }

    [[nodiscard]] const domain::model::AstNodePtr& node() const noexcept { return node_; }
    [[nodiscard]] domain::model::AstNodePtr to_ast() const noexcept { return node_; }

private:
    domain::model::AstNodePtr node_;
};

template<typename T>
concept IsExprConvertible = std::same_as<std::decay_t<T>, Expr> ||
    std::integral<std::decay_t<T>> ||
    std::floating_point<std::decay_t<T>> ||
    std::convertible_to<T, std::string> ||
    std::same_as<std::decay_t<T>, bool>;

inline Expr to_expr(const Expr& e) { return e; }
inline Expr to_expr(bool v) { return Expr(v); }
inline Expr to_expr(int v) { return Expr(v); }
inline Expr to_expr(int64_t v) { return Expr(v); }
inline Expr to_expr(double v) { return Expr(v); }
inline Expr to_expr(const std::string& v) { return Expr(v); }
inline Expr to_expr(const char* v) { return Expr(std::string(v)); }

// Factory helpers
inline Expr var(const std::string& name) {
    return Expr(std::make_shared<domain::model::VariableNode>(domain::model::Identifier(name)));
}

inline Expr lit(bool val) { return Expr(val); }
inline Expr lit(int64_t val) { return Expr(val); }
inline Expr lit(double val) { return Expr(val); }
inline Expr lit(const std::string& val) { return Expr(val); }
inline Expr lit(const char* val) { return Expr(std::string(val)); }

// Helper macro/function to generate binary operators
#define DEFINE_DSL_BINARY_OP(OP, OP_TYPE) \
    inline Expr operator OP(const Expr& lhs, const Expr& rhs) { \
        return Expr(std::make_shared<domain::model::BinaryNode>( \
            domain::model::OperatorType::OP_TYPE, lhs.node(), rhs.node())); \
    } \
    template<typename T> \
        requires (!std::same_as<std::decay_t<T>, Expr> && IsExprConvertible<T>) \
    inline Expr operator OP(const Expr& lhs, T&& rhs) { \
        return lhs OP to_expr(std::forward<T>(rhs)); \
    } \
    template<typename T> \
        requires (!std::same_as<std::decay_t<T>, Expr> && IsExprConvertible<T>) \
    inline Expr operator OP(T&& lhs, const Expr& rhs) { \
        return to_expr(std::forward<T>(lhs)) OP rhs; \
    }

DEFINE_DSL_BINARY_OP(+, Add)
DEFINE_DSL_BINARY_OP(-, Subtract)
DEFINE_DSL_BINARY_OP(*, Multiply)
DEFINE_DSL_BINARY_OP(/, Divide)
DEFINE_DSL_BINARY_OP(%, Modulo)
DEFINE_DSL_BINARY_OP(==, Equal)
DEFINE_DSL_BINARY_OP(!=, NotEqual)
DEFINE_DSL_BINARY_OP(<, Less)
DEFINE_DSL_BINARY_OP(<=, LessEqual)
DEFINE_DSL_BINARY_OP(>, Greater)
DEFINE_DSL_BINARY_OP(>=, GreaterEqual)
DEFINE_DSL_BINARY_OP(&&, And)
DEFINE_DSL_BINARY_OP(||, Or)

#undef DEFINE_DSL_BINARY_OP

inline Expr operator!(const Expr& operand) {
    return Expr(std::make_shared<domain::model::UnaryNode>(
        domain::model::OperatorType::Not, operand.node()));
}

inline Expr operator-(const Expr& operand) {
    return Expr(std::make_shared<domain::model::UnaryNode>(
        domain::model::OperatorType::Subtract, operand.node()));
}

// Fluent Rule Builder Pattern
class RuleBuilder {
public:
    RuleBuilder(std::string id, std::string name)
        : id_(std::move(id)), name_(std::move(name)) {}

    RuleBuilder& when(const Expr& condition) {
        condition_ = condition.to_ast();
        return *this;
    }

    [[nodiscard]] domain::model::AstNodePtr build_ast() const {
        if (!condition_) {
            throw std::runtime_error("RuleBuilder condition cannot be empty");
        }
        return condition_;
    }

    [[nodiscard]] const std::string& id() const noexcept { return id_; }
    [[nodiscard]] const std::string& name() const noexcept { return name_; }

private:
    std::string id_;
    std::string name_;
    domain::model::AstNodePtr condition_;
};

inline RuleBuilder rule(std::string id, std::string name) {
    return RuleBuilder(std::move(id), std::move(name));
}

} // namespace dsl::adapters::inbound
