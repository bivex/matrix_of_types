#pragma once

#include "../../domain/model/types.hpp"
#include <string>
#include <optional>

namespace dsl::application::dtos {

struct CompileRuleCommand {
    std::string rule_id;
    std::string rule_name;
    std::string dsl_script;
};

struct CompilationResultDto {
    bool is_success{false};
    std::string rule_id;
    std::string string_repr;
    std::string error_message;

    static CompilationResultDto success(std::string id, std::string repr) {
        return CompilationResultDto{true, std::move(id), std::move(repr), ""};
    }

    static CompilationResultDto failure(std::string id, std::string err) {
        return CompilationResultDto{false, std::move(id), "", std::move(err)};
    }
};

struct EvaluationResultDto {
    bool is_success{false};
    std::string rule_id;
    domain::model::Literal value;
    std::string error_message;

    static EvaluationResultDto success(std::string id, domain::model::Literal val) {
        return EvaluationResultDto{true, std::move(id), std::move(val), ""};
    }

    static EvaluationResultDto failure(std::string id, std::string err) {
        return EvaluationResultDto{false, std::move(id), domain::model::Literal{}, std::move(err)};
    }
};

} // namespace dsl::application::dtos
