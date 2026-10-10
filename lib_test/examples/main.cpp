#include "dsl/dsl.hpp"
#include <iostream>

using namespace dsl;
using namespace dsl::adapters::inbound;

int main() {
    std::cout << "\n==============================================================\n";
    std::cout << "  C++ DSL ENGINE DEMO (HEXAGONAL ARCHITECTURE + DDD)          \n";
    std::cout << "==============================================================\n\n";

    // 1. Instantiate the Hexagonal Engine (Wires Domain, Ports, and Adapters)
    DslEngine engine;

    // 2. Set runtime context (Outbound Context Repository Adapter)
    std::cout << ">>> Populating Context Variables...\n";
    engine.set("user_age", 24)
          .set("account_tier", std::string("GOLD"))
          .set("monthly_spend", 3450.75)
          .set("fraud_flag", false);

    // 3. Inbound Driving Adapter #1: Modern C++ Fluent Embedded DSL (EDSL)
    std::cout << "\n>>> [Adapter 1: C++ Fluent EDSL] Defining Discount Eligibility Rule...\n";
    auto discount_rule = rule("rule_discount_promo", "Eligible for Promo Discount")
        .when(
            (var("user_age") >= 21 && var("account_tier") == "GOLD") &&
            (var("monthly_spend") > 2000.0) &&
            !var("fraud_flag")
        );

    auto comp_res1 = engine.compile(discount_rule);
    if (comp_res1.is_success) {
        std::cout << "Compiled AST: " << comp_res1.string_repr << "\n";
    }

    // 4. Inbound Driving Adapter #2: External Script Parser (Text DSL)
    std::cout << "\n>>> [Adapter 2: Text Script DSL] Compiling Script String...\n";
    std::string text_script = "(monthly_spend * 0.05 >= 100.0) && account_tier != 'BRONZE'";
    auto comp_res2 = engine.compile_script("rule_cashback", "Cashback Calculation Rule", text_script);
    if (comp_res2.is_success) {
        std::cout << "Compiled AST from Script: " << comp_res2.string_repr << "\n";
    }

    // 5. Evaluate Rules (Application Use Cases through Driving Inbound Port)
    std::cout << "\n>>> Evaluating Rules via Application Use Cases...\n";
    auto eval1 = engine.evaluate("rule_discount_promo");
    std::cout << "Result for 'rule_discount_promo': " 
              << (eval1.is_success ? eval1.value.to_string_repr() : ("Error: " + eval1.error_message)) 
              << "\n";

    auto eval2 = engine.evaluate("rule_cashback");
    std::cout << "Result for 'rule_cashback': " 
              << (eval2.is_success ? eval2.value.to_string_repr() : ("Error: " + eval2.error_message)) 
              << "\n";

    // 6. Demonstrate Type Checker Rejecting Invalid Rule
    std::cout << "\n>>> Demonstrating Domain Type Checker Failure...\n";
    auto invalid_rule = rule("rule_invalid", "Invalid Types")
        .when(var("user_age") && lit("string_not_allowed")); // Error: && on int and string

    auto comp_invalid = engine.compile(invalid_rule);
    std::cout << "Compilation success: " << (comp_invalid.is_success ? "YES" : "NO (Rejected as expected)") << "\n";
    if (!comp_invalid.is_success) {
        std::cout << "Reason: " << comp_invalid.error_message << "\n";
    }

    std::cout << "\n==============================================================\n";
    std::cout << "  DEMO FINISHED SUCCESSFULLY!                                 \n";
    std::cout << "==============================================================\n";
    return 0;
}
