#pragma once

#include "../../domain/ports/inbound/evaluate_rule_port.hpp"
#include "../../domain/ports/outbound/rule_repository_port.hpp"
#include "../../domain/ports/outbound/context_repository_port.hpp"
#include "../../domain/ports/outbound/logger_port.hpp"
#include <memory>

namespace dsl::application::use_cases {

class EvaluateRuleUseCase final : public domain::ports::inbound::IEvaluateRulePort {
public:
    EvaluateRuleUseCase(
        std::shared_ptr<domain::ports::outbound::IRuleRepositoryPort> rule_repo,
        std::shared_ptr<domain::ports::outbound::IContextRepositoryPort> context_repo,
        std::shared_ptr<domain::ports::outbound::ILoggerPort> logger)
        : rule_repo_(std::move(rule_repo)),
          context_repo_(std::move(context_repo)),
          logger_(std::move(logger)) {}

    dtos::EvaluationResultDto evaluate_rule(const std::string& rule_id) override {
        logger_->debug("Evaluating rule with ID: " + rule_id);

        auto rule_opt = rule_repo_->find_by_id(rule_id);
        if (!rule_opt.has_value()) {
            std::string err = "Rule not found with ID: " + rule_id;
            logger_->error(err);
            return dtos::EvaluationResultDto::failure(rule_id, err);
        }

        auto rule = *rule_opt;
        try {
            // Evaluate domain aggregate against outbound context port
            auto result = rule.evaluate(*context_repo_);
            logger_->info("Rule " + rule_id + " evaluated to: " + result.to_string_repr());
            return dtos::EvaluationResultDto::success(rule_id, result);
        } catch (const std::exception& ex) {
            std::string err = "Evaluation error: " + std::string(ex.what());
            logger_->error(err);
            return dtos::EvaluationResultDto::failure(rule_id, err);
        }
    }

private:
    std::shared_ptr<domain::ports::outbound::IRuleRepositoryPort> rule_repo_;
    std::shared_ptr<domain::ports::outbound::IContextRepositoryPort> context_repo_;
    std::shared_ptr<domain::ports::outbound::ILoggerPort> logger_;
};

} // namespace dsl::application::use_cases
