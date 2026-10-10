#pragma once

#include "../../domain/model/ast_nodes.hpp"
#include "../../domain/model/types.hpp"
#include <string>
#include <string_view>
#include <vector>
#include <cctype>
#include <memory>
#include <stdexcept>

namespace dsl::adapters::inbound {

class ScriptParserAdapter {
public:
    enum class TokenType {
        Eof,
        Identifier,
        Number,
        String,
        True,
        False,
        Plus,
        Minus,
        Star,
        Slash,
        Percent,
        Caret,
        Ampersand,
        Pipe,
        Tilde,
        LeftShift,
        RightShift,
        EqualEqual,
        BangEqual,
        Less,
        LessEqual,
        Greater,
        GreaterEqual,
        AndAnd,
        OrOr,
        Bang,
        LeftParen,
        RightParen,
        Comma
    };

    struct Token {
        TokenType type;
        std::string text;
    };

    static domain::model::AstNodePtr parse(std::string_view source) {
        ScriptParserAdapter parser(source);
        return parser.parse_expression();
    }

private:
    explicit ScriptParserAdapter(std::string_view source)
        : source_(source), current_(0) {
        tokenize();
    }

    void tokenize() {
        size_t i = 0;
        while (i < source_.size()) {
            char c = source_[i];
            if (std::isspace(c)) {
                i++;
                continue;
            }

            if (std::isalpha(c) || c == '_') {
                size_t start = i;
                while (i < source_.size() && (std::isalnum(source_[i]) || source_[i] == '_')) {
                    i++;
                }
                std::string ident(source_.substr(start, i - start));
                if (ident == "true") tokens_.push_back({TokenType::True, ident});
                else if (ident == "false") tokens_.push_back({TokenType::False, ident});
                else tokens_.push_back({TokenType::Identifier, ident});
                continue;
            }

            if (std::isdigit(c)) {
                size_t start = i;
                while (i < source_.size() && (std::isdigit(source_[i]) || source_[i] == '.')) {
                    i++;
                }
                tokens_.push_back({TokenType::Number, std::string(source_.substr(start, i - start))});
                continue;
            }

            if (c == '"' || c == '\'') {
                char quote = c;
                i++;
                size_t start = i;
                while (i < source_.size() && source_[i] != quote) {
                    i++;
                }
                if (i >= source_.size()) throw std::runtime_error("Unterminated string literal");
                std::string str_val(source_.substr(start, i - start));
                i++; // skip closing quote
                tokens_.push_back({TokenType::String, str_val});
                continue;
            }

            // Two-character operators
            if (i + 1 < source_.size()) {
                std::string_view two = source_.substr(i, 2);
                if (two == "==") { tokens_.push_back({TokenType::EqualEqual, "=="}); i += 2; continue; }
                if (two == "!=") { tokens_.push_back({TokenType::BangEqual, "!="}); i += 2; continue; }
                if (two == "<=") { tokens_.push_back({TokenType::LessEqual, "<="}); i += 2; continue; }
                if (two == ">=") { tokens_.push_back({TokenType::GreaterEqual, ">="}); i += 2; continue; }
                if (two == "&&") { tokens_.push_back({TokenType::AndAnd, "&&"}); i += 2; continue; }
                if (two == "||") { tokens_.push_back({TokenType::OrOr, "||"}); i += 2; continue; }
                if (two == "<<") { tokens_.push_back({TokenType::LeftShift, "<<"}); i += 2; continue; }
                if (two == ">>") { tokens_.push_back({TokenType::RightShift, ">>"}); i += 2; continue; }
            }

            // Single-character tokens
            switch (c) {
                case '+': tokens_.push_back({TokenType::Plus, "+"}); break;
                case '-': tokens_.push_back({TokenType::Minus, "-"}); break;
                case '*': tokens_.push_back({TokenType::Star, "*"}); break;
                case '/': tokens_.push_back({TokenType::Slash, "/"}); break;
                case '%': tokens_.push_back({TokenType::Percent, "%"}); break;
                case '^': tokens_.push_back({TokenType::Caret, "^"}); break;
                case '&': tokens_.push_back({TokenType::Ampersand, "&"}); break;
                case '|': tokens_.push_back({TokenType::Pipe, "|"}); break;
                case '~': tokens_.push_back({TokenType::Tilde, "~"}); break;
                case '<': tokens_.push_back({TokenType::Less, "<"}); break;
                case '>': tokens_.push_back({TokenType::Greater, ">"}); break;
                case '!': tokens_.push_back({TokenType::Bang, "!"}); break;
                case '(': tokens_.push_back({TokenType::LeftParen, "("}); break;
                case ')': tokens_.push_back({TokenType::RightParen, ")"}); break;
                case ',': tokens_.push_back({TokenType::Comma, ","}); break;
                default:
                    throw std::runtime_error(std::string("Unexpected character in DSL: '") + c + "'");
            }
            i++;
        }
        tokens_.push_back({TokenType::Eof, ""});
    }

    domain::model::AstNodePtr parse_expression() {
        return parse_or();
    }

    domain::model::AstNodePtr parse_or() {
        auto expr = parse_and();
        while (match(TokenType::OrOr)) {
            auto right = parse_and();
            expr = std::make_shared<domain::model::BinaryNode>(
                domain::model::OperatorType::Or, expr, right);
        }
        return expr;
    }

    domain::model::AstNodePtr parse_and() {
        auto expr = parse_bitwise_or();
        while (match(TokenType::AndAnd)) {
            auto right = parse_bitwise_or();
            expr = std::make_shared<domain::model::BinaryNode>(
                domain::model::OperatorType::And, expr, right);
        }
        return expr;
    }

    domain::model::AstNodePtr parse_bitwise_or() {
        auto expr = parse_bitwise_and();
        while (match(TokenType::Pipe)) {
            auto right = parse_bitwise_and();
            expr = std::make_shared<domain::model::BinaryNode>(
                domain::model::OperatorType::BitOr, expr, right);
        }
        return expr;
    }

    domain::model::AstNodePtr parse_bitwise_and() {
        auto expr = parse_equality();
        while (match(TokenType::Ampersand)) {
            auto right = parse_equality();
            expr = std::make_shared<domain::model::BinaryNode>(
                domain::model::OperatorType::BitAnd, expr, right);
        }
        return expr;
    }

    domain::model::AstNodePtr parse_equality() {
        auto expr = parse_relational();
        while (check(TokenType::EqualEqual) || check(TokenType::BangEqual)) {
            auto op_token = advance();
            auto op = (op_token.type == TokenType::EqualEqual)
                ? domain::model::OperatorType::Equal
                : domain::model::OperatorType::NotEqual;
            auto right = parse_relational();
            expr = std::make_shared<domain::model::BinaryNode>(op, expr, right);
        }
        return expr;
    }

    domain::model::AstNodePtr parse_relational() {
        auto expr = parse_shift();
        while (check(TokenType::Less) || check(TokenType::LessEqual) ||
               check(TokenType::Greater) || check(TokenType::GreaterEqual)) {
            auto op_token = advance();
            domain::model::OperatorType op;
            if (op_token.type == TokenType::Less) op = domain::model::OperatorType::Less;
            else if (op_token.type == TokenType::LessEqual) op = domain::model::OperatorType::LessEqual;
            else if (op_token.type == TokenType::Greater) op = domain::model::OperatorType::Greater;
            else op = domain::model::OperatorType::GreaterEqual;

            auto right = parse_shift();
            expr = std::make_shared<domain::model::BinaryNode>(op, expr, right);
        }
        return expr;
    }

    domain::model::AstNodePtr parse_shift() {
        auto expr = parse_additive();
        while (check(TokenType::LeftShift) || check(TokenType::RightShift)) {
            auto op_token = advance();
            auto op = (op_token.type == TokenType::LeftShift)
                ? domain::model::OperatorType::ShiftLeft
                : domain::model::OperatorType::ShiftRight;
            auto right = parse_additive();
            expr = std::make_shared<domain::model::BinaryNode>(op, expr, right);
        }
        return expr;
    }

    domain::model::AstNodePtr parse_additive() {
        auto expr = parse_multiplicative();
        while (check(TokenType::Plus) || check(TokenType::Minus)) {
            auto op_token = advance();
            auto op = (op_token.type == TokenType::Plus)
                ? domain::model::OperatorType::Add
                : domain::model::OperatorType::Subtract;
            auto right = parse_multiplicative();
            expr = std::make_shared<domain::model::BinaryNode>(op, expr, right);
        }
        return expr;
    }

    domain::model::AstNodePtr parse_multiplicative() {
        auto expr = parse_power();
        while (check(TokenType::Star) || check(TokenType::Slash) || check(TokenType::Percent)) {
            auto op_token = advance();
            domain::model::OperatorType op;
            if (op_token.type == TokenType::Star) op = domain::model::OperatorType::Multiply;
            else if (op_token.type == TokenType::Slash) op = domain::model::OperatorType::Divide;
            else op = domain::model::OperatorType::Modulo;

            auto right = parse_power();
            expr = std::make_shared<domain::model::BinaryNode>(op, expr, right);
        }
        return expr;
    }

    domain::model::AstNodePtr parse_power() {
        auto expr = parse_unary();
        if (match(TokenType::Caret)) {
            auto right = parse_power(); // Right-associative: 2 ^ 3 ^ 2
            return std::make_shared<domain::model::BinaryNode>(
                domain::model::OperatorType::Power, expr, right);
        }
        return expr;
    }

    domain::model::AstNodePtr parse_unary() {
        if (match(TokenType::Bang)) {
            auto operand = parse_unary();
            return std::make_shared<domain::model::UnaryNode>(
                domain::model::OperatorType::Not, operand);
        }
        if (match(TokenType::Minus)) {
            auto operand = parse_unary();
            return std::make_shared<domain::model::UnaryNode>(
                domain::model::OperatorType::Subtract, operand);
        }
        if (match(TokenType::Tilde)) {
            auto operand = parse_unary();
            return std::make_shared<domain::model::UnaryNode>(
                domain::model::OperatorType::BitNot, operand);
        }
        return parse_primary();
    }

    domain::model::AstNodePtr parse_primary() {
        if (match(TokenType::True)) {
            return std::make_shared<domain::model::LiteralNode>(domain::model::Literal(true));
        }
        if (match(TokenType::False)) {
            return std::make_shared<domain::model::LiteralNode>(domain::model::Literal(false));
        }
        if (check(TokenType::Number)) {
            auto tok = advance();
            if (tok.text.find('.') != std::string::npos) {
                return std::make_shared<domain::model::LiteralNode>(domain::model::Literal(std::stod(tok.text)));
            } else {
                return std::make_shared<domain::model::LiteralNode>(domain::model::Literal(std::stoll(tok.text)));
            }
        }
        if (check(TokenType::String)) {
            auto tok = advance();
            return std::make_shared<domain::model::LiteralNode>(domain::model::Literal(tok.text));
        }
        if (check(TokenType::Identifier)) {
            auto ident_token = advance();
            // Check for function call
            if (match(TokenType::LeftParen)) {
                std::vector<domain::model::AstNodePtr> args;
                if (!check(TokenType::RightParen)) {
                    do {
                        args.push_back(parse_expression());
                    } while (match(TokenType::Comma));
                }
                consume(TokenType::RightParen, "Expected ')' after function arguments");
                return std::make_shared<domain::model::FunctionCallNode>(ident_token.text, std::move(args));
            }
            return std::make_shared<domain::model::VariableNode>(domain::model::Identifier(ident_token.text));
        }
        if (match(TokenType::LeftParen)) {
            auto expr = parse_expression();
            consume(TokenType::RightParen, "Expected ')' after grouped expression");
            return expr;
        }

        throw std::runtime_error("Unexpected token in expression: '" + peek().text + "'");
    }

    bool match(TokenType t) {
        if (check(t)) {
            advance();
            return true;
        }
        return false;
    }

    bool check(TokenType t) const {
        if (is_at_end()) return false;
        return peek().type == t;
    }

    const Token& advance() {
        if (!is_at_end()) current_++;
        return previous();
    }

    bool is_at_end() const {
        return peek().type == TokenType::Eof;
    }

    const Token& peek() const {
        return tokens_[current_];
    }

    const Token& previous() const {
        return tokens_[current_ - 1];
    }

    void consume(TokenType t, const std::string& err) {
        if (check(t)) {
            advance();
            return;
        }
        throw std::runtime_error(err);
    }

    std::string_view source_;
    std::vector<Token> tokens_;
    size_t current_{0};
};

} // namespace dsl::adapters::inbound
