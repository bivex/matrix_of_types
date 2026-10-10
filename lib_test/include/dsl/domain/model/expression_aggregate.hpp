#pragma once

#include "ast_nodes.hpp"
#include "../events/domain_events.hpp"
#include "../services/evaluator_service.hpp"
#include <string>
#include <vector>
#include <memory>

namespace dsl::domain::model {

class ExpressionAggregate {
public:
    ExpressionAggregate(std::string id, std::string name, AstNodePtr ast_root)
        : id_(std::move(id)), name_(std::move(name)), ast_root_(std::move(ast_root)) {
        if (id_.empty()) {
            throw std::invalid_argument("Aggregate ID cannot be empty");
        }
        if (!ast_root_) {
            throw std::invalid_argument("Aggregate root AST node cannot be null");
        }
        record_event(std::make_shared<events::RuleCompiledEvent>(id_, name_));
    }

    [[nodiscard]] const std::string& id() const noexcept { return id_; }
    [[nodiscard]] const std::string& name() const noexcept { return name_; }
    [[nodiscard]] const AstNodePtr& root() const noexcept { return ast_root_; }

    Literal evaluate(const ports::outbound::IContextRepositoryPort& context) {
        services::ExpressionEvaluatorService evaluator(context);
        auto result = evaluator.evaluate(*ast_root_);
        record_event(std::make_shared<events::RuleEvaluatedEvent>(id_, result));
        return result;
    }

    [[nodiscard]] std::vector<events::DomainEventPtr> pull_events() {
        auto events = std::move(domain_events_);
        domain_events_.clear();
        return events;
    }

private:
    void record_event(events::DomainEventPtr event) {
        domain_events_.push_back(std::move(event));
    }

    std::string id_;
    std::string name_;
    AstNodePtr ast_root_;
    std::vector<events::DomainEventPtr> domain_events_;
};

} // namespace dsl::domain::model
