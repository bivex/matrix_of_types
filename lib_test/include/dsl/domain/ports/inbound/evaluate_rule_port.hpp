#pragma once

#include "../../../application/dtos/rule_dto.hpp"
#include <string>

namespace dsl::domain::ports::inbound {

class IEvaluateRulePort {
public:
    virtual ~IEvaluateRulePort() = default;
    virtual application::dtos::EvaluationResultDto evaluate_rule(const std::string& rule_id) = 0;
};

} // namespace dsl::domain::ports::inbound
