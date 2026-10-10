#pragma once

#include "../../domain/ports/inbound/compile_rule_port.hpp"
#include "../../domain/ports/outbound/rule_repository_port.hpp"
#include "../../domain/ports/outbound/logger_port.hpp"
#include "../../domain/services/type_checker_service.hpp"
#include <memory>

namespace dsl::application::use_cases {

class CompileRuleUseCase final : public domain::ports::inbound::ICompileRulePort {
public:
    CompileRuleUseCase(
        std::shared_ptr<domain::ports::outbound::IRuleRepositoryPort> repository,
        std::shared_ptr<domain::ports::outbound::ILoggerPort> logger)
        : repository_(std::move(repository)), logger_(std::move(logger)) {}

    dtos::CompilationResultDto compile_from_ast(
        const std::string& rule_id,
        const std::string& rule_name,
        domain::model::AstNodePtr ast) override {
        
        logger_->info("Compiling DSL rule: " + rule_id + " ('" + rule_name + "')");

        if (!ast) {
            std::string err = "AST root cannot be null";
            logger_->error(err);
            return dtos::CompilationResultDto::failure(rule_id, err);
        }

        // Domain Service: Type check
        domain::services::TypeCheckerService type_checker;
        auto check_res = type_checker.validate(*ast);
        if (!check_res.is_valid) {
            std::string err = "Type check failed: " + check_res.error_message;
            logger_->error(err);
            return dtos::CompilationResultDto::failure(rule_id, err);
        }

        try {
            // Create Aggregate Root
            domain::model::ExpressionAggregate aggregate(rule_id, rule_name, ast);
            std::string repr = ast->to_string();

            // Save to Outbound Port
            repository_->save(std::move(aggregate));
            logger_->info("Rule compiled successfully: " + repr);

            return dtos::CompilationResultDto::success(rule_id, repr);
        } catch (const std::exception& ex) {
            std::string err = "Aggregate creation failed: " + std::string(ex.what());
            logger_->error(err);
            return dtos::CompilationResultDto::failure(rule_id, err);
        }
    }

private:
    std::shared_ptr<domain::ports::outbound::IRuleRepositoryPort> repository_;
    std::shared_ptr<domain::ports::outbound::ILoggerPort> logger_;
};

} // namespace dsl::application::use_cases
