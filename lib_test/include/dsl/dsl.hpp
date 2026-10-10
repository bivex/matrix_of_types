#pragma once

// Domain
#include "domain/model/types.hpp"
#include "domain/model/ast_nodes.hpp"
#include "domain/model/expression_aggregate.hpp"
#include "domain/events/domain_events.hpp"
#include "domain/services/evaluator_service.hpp"
#include "domain/services/type_checker_service.hpp"
#include "domain/ports/inbound/compile_rule_port.hpp"
#include "domain/ports/inbound/evaluate_rule_port.hpp"
#include "domain/ports/outbound/context_repository_port.hpp"
#include "domain/ports/outbound/rule_repository_port.hpp"
#include "domain/ports/outbound/logger_port.hpp"

// Application
#include "application/dtos/rule_dto.hpp"
#include "application/use_cases/compile_rule_use_case.hpp"
#include "application/use_cases/evaluate_rule_use_case.hpp"

// Adapters
#include "adapters/inbound/fluent_edsl_adapter.hpp"
#include "adapters/inbound/script_parser_adapter.hpp"
#include "adapters/outbound/in_memory_rule_repository.hpp"
#include "adapters/outbound/in_memory_context_adapter.hpp"
#include "adapters/outbound/console_logger_adapter.hpp"

#include <memory>

namespace dsl {

// Hexagonal DDD Engine Context Facade
class DslEngine {
public:
    DslEngine() {
        rule_repo_ = std::make_shared<adapters::outbound::InMemoryRuleRepositoryAdapter>();
        context_repo_ = std::make_shared<adapters::outbound::InMemoryContextAdapter>();
        logger_ = std::make_shared<adapters::outbound::ConsoleLoggerAdapter>();

        compiler_ = std::make_shared<application::use_cases::CompileRuleUseCase>(rule_repo_, logger_);
        evaluator_ = std::make_shared<application::use_cases::EvaluateRuleUseCase>(rule_repo_, context_repo_, logger_);
    }

    // Set variable in execution context
    template<typename T>
    DslEngine& set(const std::string& name, T&& val) {
        context_repo_->set_variable(name, domain::model::Literal(std::forward<T>(val)));
        return *this;
    }

    // Compile from C++ Fluent EDSL
    application::dtos::CompilationResultDto compile(const adapters::inbound::RuleBuilder& builder) {
        return compiler_->compile_from_ast(builder.id(), builder.name(), builder.build_ast());
    }

    // Compile from raw AST
    application::dtos::CompilationResultDto compile(const std::string& id, const std::string& name, domain::model::AstNodePtr ast) {
        return compiler_->compile_from_ast(id, name, std::move(ast));
    }

    // Compile from external text script DSL
    application::dtos::CompilationResultDto compile_script(const std::string& id, const std::string& name, std::string_view script) {
        auto ast = adapters::inbound::ScriptParserAdapter::parse(script);
        return compiler_->compile_from_ast(id, name, std::move(ast));
    }

    // Evaluate compiled rule
    application::dtos::EvaluationResultDto evaluate(const std::string& rule_id) {
        return evaluator_->evaluate_rule(rule_id);
    }

    // Direct access to ports if custom adapters are used
    [[nodiscard]] auto context() const { return context_repo_; }
    [[nodiscard]] auto repository() const { return rule_repo_; }
    [[nodiscard]] auto logger() const { return logger_; }

private:
    std::shared_ptr<adapters::outbound::InMemoryRuleRepositoryAdapter> rule_repo_;
    std::shared_ptr<adapters::outbound::InMemoryContextAdapter> context_repo_;
    std::shared_ptr<adapters::outbound::ConsoleLoggerAdapter> logger_;

    std::shared_ptr<application::use_cases::CompileRuleUseCase> compiler_;
    std::shared_ptr<application::use_cases::EvaluateRuleUseCase> evaluator_;
};

} // namespace dsl
