#pragma once

#include <string>
#include <variant>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <regex>

namespace dsl::domain::model {

enum class DslType {
    Null,
    Boolean,
    Integer,
    Float,
    String
};

inline std::string to_string(DslType type) {
    switch (type) {
        case DslType::Null:    return "Null";
        case DslType::Boolean: return "Boolean";
        case DslType::Integer: return "Integer";
        case DslType::Float:   return "Float";
        case DslType::String:  return "String";
    }
    return "Unknown";
}

enum class OperatorType {
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    Power,
    BitAnd,
    BitOr,
    BitXor,
    BitNot,
    ShiftLeft,
    ShiftRight,
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    And,
    Or,
    Not
};

inline std::string to_string(OperatorType op) {
    switch (op) {
        case OperatorType::Add:          return "+";
        case OperatorType::Subtract:     return "-";
        case OperatorType::Multiply:     return "*";
        case OperatorType::Divide:       return "/";
        case OperatorType::Modulo:       return "%";
        case OperatorType::Power:        return "^";
        case OperatorType::BitAnd:       return "&";
        case OperatorType::BitOr:        return "|";
        case OperatorType::BitXor:       return "^^";
        case OperatorType::BitNot:       return "~";
        case OperatorType::ShiftLeft:    return "<<";
        case OperatorType::ShiftRight:   return ">>";
        case OperatorType::Equal:        return "==";
        case OperatorType::NotEqual:     return "!=";
        case OperatorType::Less:         return "<";
        case OperatorType::LessEqual:    return "<=";
        case OperatorType::Greater:      return ">";
        case OperatorType::GreaterEqual: return ">=";
        case OperatorType::And:          return "&&";
        case OperatorType::Or:           return "||";
        case OperatorType::Not:          return "!";
    }
    return "?";
}

// Value Object: Literal Value
class Literal {
public:
    using ValueVariant = std::variant<std::monostate, bool, int64_t, double, std::string>;

    Literal() : value_(std::monostate{}) {}
    explicit Literal(bool v) : value_(v) {}
    explicit Literal(int v) : value_(static_cast<int64_t>(v)) {}
    explicit Literal(int64_t v) : value_(v) {}
    explicit Literal(double v) : value_(v) {}
    explicit Literal(std::string v) : value_(std::move(v)) {}
    explicit Literal(const char* v) : value_(std::string(v)) {}

    [[nodiscard]] DslType type() const {
        if (std::holds_alternative<bool>(value_)) return DslType::Boolean;
        if (std::holds_alternative<int64_t>(value_)) return DslType::Integer;
        if (std::holds_alternative<double>(value_)) return DslType::Float;
        if (std::holds_alternative<std::string>(value_)) return DslType::String;
        return DslType::Null;
    }

    [[nodiscard]] bool is_null() const { return std::holds_alternative<std::monostate>(value_); }
    [[nodiscard]] bool is_bool() const { return std::holds_alternative<bool>(value_); }
    [[nodiscard]] bool is_int() const { return std::holds_alternative<int64_t>(value_); }
    [[nodiscard]] bool is_float() const { return std::holds_alternative<double>(value_); }
    [[nodiscard]] bool is_number() const { return is_int() || is_float(); }
    [[nodiscard]] bool is_string() const { return std::holds_alternative<std::string>(value_); }

    [[nodiscard]] bool as_bool() const {
        if (!is_bool()) throw std::runtime_error("Literal is not a Boolean");
        return std::get<bool>(value_);
    }

    [[nodiscard]] int64_t as_int() const {
        if (!is_int()) throw std::runtime_error("Literal is not an Integer");
        return std::get<int64_t>(value_);
    }

    [[nodiscard]] double as_float() const {
        if (is_float()) return std::get<double>(value_);
        if (is_int()) return static_cast<double>(std::get<int64_t>(value_));
        throw std::runtime_error("Literal is not a Number");
    }

    [[nodiscard]] double to_number() const {
        return as_float();
    }

    [[nodiscard]] const std::string& as_string() const {
        if (!is_string()) throw std::runtime_error("Literal is not a String");
        return std::get<std::string>(value_);
    }

    [[nodiscard]] std::string to_string_repr() const {
        if (is_null()) return "null";
        if (is_bool()) return as_bool() ? "true" : "false";
        if (is_int()) return std::to_string(as_int());
        if (is_float()) {
            std::ostringstream ss;
            ss << as_float();
            return ss.str();
        }
        if (is_string()) return "\"" + as_string() + "\"";
        return "unknown";
    }

    bool operator==(const Literal& other) const {
        if (type() != other.type()) {
            if (is_number() && other.is_number()) {
                return to_number() == other.to_number();
            }
            return false;
        }
        return value_ == other.value_;
    }

    bool operator!=(const Literal& other) const {
        return !(*this == other);
    }

private:
    ValueVariant value_;
};

// Value Object: Identifier (Variable, Column Name, Function Name)
class Identifier {
public:
    explicit Identifier(std::string name) : name_(std::move(name)) {
        validate();
    }

    [[nodiscard]] const std::string& value() const noexcept { return name_; }

    bool operator==(const Identifier& other) const noexcept {
        return name_ == other.name_;
    }

    bool operator<(const Identifier& other) const noexcept {
        return name_ < other.name_;
    }

private:
    void validate() const {
        if (name_.empty()) {
            throw std::invalid_argument("Identifier cannot be empty");
        }
        if (!std::isalpha(name_[0]) && name_[0] != '_') {
            throw std::invalid_argument("Identifier must start with a letter or underscore: '" + name_ + "'");
        }
        for (char c : name_) {
            if (!std::isalnum(c) && c != '_') {
                throw std::invalid_argument("Identifier contains invalid character '" + std::string(1, c) + "' in: '" + name_ + "'");
            }
        }
    }

    std::string name_;
};

} // namespace dsl::domain::model
