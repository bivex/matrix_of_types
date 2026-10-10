#pragma once

#include "../../model/expression_aggregate.hpp"
#include <optional>
#include <string>
#include <memory>

namespace dsl::domain::ports::outbound {

class IRuleRepositoryPort {
public:
    virtual ~IRuleRepositoryPort() = default;
    virtual void save(model::ExpressionAggregate rule) = 0;
    [[nodiscard]] virtual std::optional<model::ExpressionAggregate> find_by_id(const std::string& id) const = 0;
    [[nodiscard]] virtual bool exists(const std::string& id) const = 0;
};

} // namespace dsl::domain::ports::outbound
