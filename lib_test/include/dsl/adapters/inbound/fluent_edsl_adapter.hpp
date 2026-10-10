#pragma once

#include "../../domain/model/ast_nodes.hpp"
#include "../../domain/model/types.hpp"
#include <memory>
#include <string>
#include <vector>
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
inline Expr lit(int val) { return Expr(val); }
inline Expr lit(int64_t val) { return Expr(val); }
inline Expr lit(double val) { return Expr(val); }
inline Expr lit(const std::string& val) { return Expr(val); }
inline Expr lit(const char* val) { return Expr(std::string(val)); }

// Universal function call node creator
template<typename... Args>
inline Expr call_fn(const std::string& name, Args&&... args) {
    std::vector<domain::model::AstNodePtr> ast_args;
    (ast_args.push_back(to_expr(std::forward<Args>(args)).to_ast()), ...);
    return Expr(std::make_shared<domain::model::FunctionCallNode>(name, std::move(ast_args)));
}

// Binary Operator Definitions
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
DEFINE_DSL_BINARY_OP(&, BitAnd)
DEFINE_DSL_BINARY_OP(|, BitOr)
DEFINE_DSL_BINARY_OP(^, Power)
DEFINE_DSL_BINARY_OP(<<, ShiftLeft)
DEFINE_DSL_BINARY_OP(>>, ShiftRight)
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

inline Expr operator~(const Expr& operand) {
    return Expr(std::make_shared<domain::model::UnaryNode>(
        domain::model::OperatorType::BitNot, operand.node()));
}

// -------------------------------------------------------------
// Exhaustive Mathematical Functions for C++ EDSL
// -------------------------------------------------------------

// Roots & Powers
template<IsExprConvertible T>
inline Expr sqrt(const T& x) { return call_fn("sqrt", to_expr(x)); }

template<IsExprConvertible T>
inline Expr cbrt(const T& x) { return call_fn("cbrt", to_expr(x)); }

template<IsExprConvertible T1, IsExprConvertible T2>
inline Expr pow(const T1& x, const T2& y) { return call_fn("pow", to_expr(x), to_expr(y)); }

template<IsExprConvertible T1, IsExprConvertible T2>
inline Expr hypot(const T1& x, const T2& y) { return call_fn("hypot", to_expr(x), to_expr(y)); }

// Exponential & Logarithms
template<IsExprConvertible T>
inline Expr exp(const T& x) { return call_fn("exp", to_expr(x)); }

template<IsExprConvertible T>
inline Expr exp2(const T& x) { return call_fn("exp2", to_expr(x)); }

template<IsExprConvertible T>
inline Expr log(const T& x) { return call_fn("log", to_expr(x)); }

template<IsExprConvertible T>
inline Expr ln(const T& x) { return call_fn("log", to_expr(x)); }

template<IsExprConvertible T>
inline Expr log10(const T& x) { return call_fn("log10", to_expr(x)); }

template<IsExprConvertible T>
inline Expr log2(const T& x) { return call_fn("log2", to_expr(x)); }

// Trigonometric Functions
template<IsExprConvertible T>
inline Expr sin(const T& x) { return call_fn("sin", to_expr(x)); }

template<IsExprConvertible T>
inline Expr cos(const T& x) { return call_fn("cos", to_expr(x)); }

template<IsExprConvertible T>
inline Expr tan(const T& x) { return call_fn("tan", to_expr(x)); }

template<IsExprConvertible T>
inline Expr asin(const T& x) { return call_fn("asin", to_expr(x)); }

template<IsExprConvertible T>
inline Expr acos(const T& x) { return call_fn("acos", to_expr(x)); }

template<IsExprConvertible T>
inline Expr atan(const T& x) { return call_fn("atan", to_expr(x)); }

template<IsExprConvertible T1, IsExprConvertible T2>
inline Expr atan2(const T1& y, const T2& x) { return call_fn("atan2", to_expr(y), to_expr(x)); }

// Hyperbolic Functions
template<IsExprConvertible T>
inline Expr sinh(const T& x) { return call_fn("sinh", to_expr(x)); }

template<IsExprConvertible T>
inline Expr cosh(const T& x) { return call_fn("cosh", to_expr(x)); }

template<IsExprConvertible T>
inline Expr tanh(const T& x) { return call_fn("tanh", to_expr(x)); }

template<IsExprConvertible T>
inline Expr asinh(const T& x) { return call_fn("asinh", to_expr(x)); }

template<IsExprConvertible T>
inline Expr acosh(const T& x) { return call_fn("acosh", to_expr(x)); }

template<IsExprConvertible T>
inline Expr atanh(const T& x) { return call_fn("atanh", to_expr(x)); }

// Rounding, Signs & Absolute Value
template<IsExprConvertible T>
inline Expr abs(const T& x) { return call_fn("abs", to_expr(x)); }

template<IsExprConvertible T>
inline Expr sign(const T& x) { return call_fn("sign", to_expr(x)); }

template<IsExprConvertible T>
inline Expr sgn(const T& x) { return call_fn("sign", to_expr(x)); }

template<IsExprConvertible T>
inline Expr floor(const T& x) { return call_fn("floor", to_expr(x)); }

template<IsExprConvertible T>
inline Expr ceil(const T& x) { return call_fn("ceil", to_expr(x)); }

template<IsExprConvertible T>
inline Expr round(const T& x) { return call_fn("round", to_expr(x)); }

template<IsExprConvertible T>
inline Expr trunc(const T& x) { return call_fn("trunc", to_expr(x)); }

// Extrema & Bounding
template<IsExprConvertible T1, IsExprConvertible T2>
inline Expr min(const T1& a, const T2& b) { return call_fn("min", to_expr(a), to_expr(b)); }

template<IsExprConvertible T1, IsExprConvertible T2>
inline Expr max(const T1& a, const T2& b) { return call_fn("max", to_expr(a), to_expr(b)); }

template<IsExprConvertible T1, IsExprConvertible T2, IsExprConvertible T3>
inline Expr clamp(const T1& val, const T2& lo, const T3& hi) { return call_fn("clamp", to_expr(val), to_expr(lo), to_expr(hi)); }

// Discrete & Integer Mathematics
template<IsExprConvertible T1, IsExprConvertible T2>
inline Expr gcd(const T1& a, const T2& b) { return call_fn("gcd", to_expr(a), to_expr(b)); }

template<IsExprConvertible T1, IsExprConvertible T2>
inline Expr lcm(const T1& a, const T2& b) { return call_fn("lcm", to_expr(a), to_expr(b)); }

template<IsExprConvertible T>
inline Expr factorial(const T& n) { return call_fn("factorial", to_expr(n)); }

// Special Functions
template<IsExprConvertible T>
inline Expr erf(const T& x) { return call_fn("erf", to_expr(x)); }

template<IsExprConvertible T>
inline Expr gamma(const T& x) { return call_fn("gamma", to_expr(x)); }

// Mathematical Constants
inline Expr pi() { return lit(3.14159265358979323846); }
inline Expr e_const() { return lit(2.71828182845904523536); }
inline Expr tau() { return lit(6.28318530717958647692); }

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
