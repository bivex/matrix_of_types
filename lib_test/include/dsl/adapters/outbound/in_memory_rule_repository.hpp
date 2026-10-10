#pragma once

#include "../../domain/ports/outbound/rule_repository_port.hpp"
#include <unordered_map>
#include <shared_mutex>

namespace dsl::adapters::outbound {

class InMemoryRuleRepositoryAdapter final : public domain::ports::outbound::IRuleRepositoryPort {
public:
    void save(domain::model::ExpressionAggregate rule) override {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        rules_.insert_or_assign(rule.id(), std::move(rule));
    }

    std::optional<domain::model::ExpressionAggregate> find_by_id(const std::string& id) const override {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto it = rules_.find(id);
        if (it != rules_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    bool exists(const std::string& id) const override {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return rules_.find(id) != rules_.end();
    }

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, domain::model::ExpressionAggregate> rules_;
};

} // namespace dsl::adapters::outbound
