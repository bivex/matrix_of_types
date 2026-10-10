#pragma once

#include "../../model/types.hpp"
#include <optional>
#include <string>

namespace dsl::domain::ports::outbound {

class IContextRepositoryPort {
public:
    virtual ~IContextRepositoryPort() = default;
    [[nodiscard]] virtual std::optional<model::Literal> get_variable(const std::string& name) const = 0;
    virtual void set_variable(const std::string& name, model::Literal value) = 0;
    [[nodiscard]] virtual bool has_variable(const std::string& name) const = 0;
};

} // namespace dsl::domain::ports::outbound
