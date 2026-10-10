#pragma once

#include "../../domain/ports/outbound/context_repository_port.hpp"
#include <unordered_map>
#include <shared_mutex>

namespace dsl::adapters::outbound {

class InMemoryContextAdapter final : public domain::ports::outbound::IContextRepositoryPort {
public:
    std::optional<domain::model::Literal> get_variable(const std::string& name) const override {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto it = variables_.find(name);
        if (it != variables_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    void set_variable(const std::string& name, domain::model::Literal value) override {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        variables_[name] = std::move(value);
    }

    bool has_variable(const std::string& name) const override {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return variables_.find(name) != variables_.end();
    }

    // Helper fluent method for test setups
    InMemoryContextAdapter& with(const std::string& name, domain::model::Literal value) {
        set_variable(name, std::move(value));
        return *this;
    }

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, domain::model::Literal> variables_;
};

} // namespace dsl::adapters::outbound
