#pragma once

#include <string>
#include <chrono>
#include <memory>
#include "../model/types.hpp"

namespace dsl::domain::events {

class IDomainEvent {
public:
    virtual ~IDomainEvent() = default;
    [[nodiscard]] virtual std::string event_name() const = 0;
    [[nodiscard]] std::chrono::system_clock::time_point timestamp() const noexcept { return timestamp_; }

protected:
    IDomainEvent() : timestamp_(std::chrono::system_clock::now()) {}

private:
    std::chrono::system_clock::time_point timestamp_;
};

using DomainEventPtr = std::shared_ptr<const IDomainEvent>;

class RuleCompiledEvent final : public IDomainEvent {
public:
    RuleCompiledEvent(std::string rule_id, std::string rule_name)
        : rule_id_(std::move(rule_id)), rule_name_(std::move(rule_name)) {}

    [[nodiscard]] std::string event_name() const override { return "RuleCompiledEvent"; }
    [[nodiscard]] const std::string& rule_id() const noexcept { return rule_id_; }
    [[nodiscard]] const std::string& rule_name() const noexcept { return rule_name_; }

private:
    std::string rule_id_;
    std::string rule_name_;
};

class RuleEvaluatedEvent final : public IDomainEvent {
public:
    RuleEvaluatedEvent(std::string rule_id, model::Literal result)
        : rule_id_(std::move(rule_id)), result_(std::move(result)) {}

    [[nodiscard]] std::string event_name() const override { return "RuleEvaluatedEvent"; }
    [[nodiscard]] const std::string& rule_id() const noexcept { return rule_id_; }
    [[nodiscard]] const model::Literal& result() const noexcept { return result_; }

private:
    std::string rule_id_;
    model::Literal result_;
};

class CompilationFailedEvent final : public IDomainEvent {
public:
    CompilationFailedEvent(std::string rule_id, std::string error_message)
        : rule_id_(std::move(rule_id)), error_message_(std::move(error_message)) {}

    [[nodiscard]] std::string event_name() const override { return "CompilationFailedEvent"; }
    [[nodiscard]] const std::string& rule_id() const noexcept { return rule_id_; }
    [[nodiscard]] const std::string& error_message() const noexcept { return error_message_; }

private:
    std::string rule_id_;
    std::string error_message_;
};

} // namespace dsl::domain::events
