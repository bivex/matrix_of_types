#pragma once

#include "../../model/ast_nodes.hpp"
#include "../../../application/dtos/rule_dto.hpp"
#include <string>

namespace dsl::domain::ports::inbound {

class ICompileRulePort {
public:
    virtual ~ICompileRulePort() = default;
    virtual application::dtos::CompilationResultDto compile_from_ast(
        const std::string& rule_id,
        const std::string& rule_name,
        model::AstNodePtr ast) = 0;
};

} // namespace dsl::domain::ports::inbound
