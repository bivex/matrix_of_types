#include "dsl/dsl.hpp"
#include <cassert>
#include <iostream>

using namespace dsl;
using namespace dsl::adapters::inbound;
using namespace dsl::domain::model;

void test_value_objects() {
    std::cout << "--- Running test_value_objects ---\n";
    Literal l_int(42);
    assert(l_int.is_int());
    assert(l_int.as_int() == 42);
    assert(l_int.to_number() == 42.0);

    Literal l_flt(3.14);
    assert(l_flt.is_float());
    assert(l_flt.as_float() == 3.14);

    Literal l_str("hello");
    assert(l_str.is_string());
    assert(l_str.as_string() == "hello");

    Literal l_bool(true);
    assert(l_bool.is_bool());
    assert(l_bool.as_bool() == true);

    // Equality with numeric promotion
    assert(Literal(42) == Literal(42.0));
    assert(Literal(1) != Literal(2));

    // Identifier validation
    Identifier valid_id("user_age_1");
    assert(valid_id.value() == "user_age_1");

    bool threw = false;
    try {
        Identifier invalid_id("123_invalid");
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    std::cout << "test_value_objects PASSED!\n";
}

void test_type_checker() {
    std::cout << "--- Running test_type_checker ---\n";
    domain::services::TypeCheckerService checker;

    // Valid boolean condition: age > 18
    auto ast1 = (var("age") > 18).to_ast();
    auto res1 = checker.validate(*ast1);
    assert(res1.is_valid);
    assert(res1.inferred_type == DslType::Boolean);

    // Invalid boolean operation: "hello" && true
    auto ast2 = (lit("hello") && lit(true)).to_ast();
    auto res2 = checker.validate(*ast2);
    assert(!res2.is_valid);
    std::cout << "Type error caught as expected: " << res2.error_message << "\n";

    std::cout << "test_type_checker PASSED!\n";
}

void test_fluent_edsl() {
    std::cout << "--- Running test_fluent_edsl ---\n";
    DslEngine engine;

    engine.set("age", 25)
          .set("status", std::string("active"))
          .set("balance", 1500.50);

    // Rule 1: age >= 18 && status == "active"
    auto rule1 = rule("rule_adult_active", "Check user access")
                    .when(var("age") >= 18 && var("status") == "active");

    auto comp1 = engine.compile(rule1);
    assert(comp1.is_success);

    auto eval1 = engine.evaluate("rule_adult_active");
    assert(eval1.is_success);
    assert(eval1.value.as_bool() == true);

    // Rule 2: balance * 1.1 > 2000
    auto rule2 = rule("rule_vip_balance", "Check VIP status")
                    .when(var("balance") * 1.1 > 2000.0);

    auto comp2 = engine.compile(rule2);
    assert(comp2.is_success);

    auto eval2 = engine.evaluate("rule_vip_balance");
    assert(eval2.is_success);
    assert(eval2.value.as_bool() == false); // 1500.5 * 1.1 = 1650.55 <= 2000

    std::cout << "test_fluent_edsl PASSED!\n";
}

void test_script_parser() {
    std::cout << "--- Running test_script_parser ---\n";
    DslEngine engine;

    engine.set("score", 85)
          .set("is_admin", false)
          .set("department", std::string("IT"));

    // Text script DSL
    std::string script = "(score >= 80 && department == 'IT') || is_admin == true";
    auto comp = engine.compile_script("rule_script_1", "Text Script Test", script);
    assert(comp.is_success);

    auto eval = engine.evaluate("rule_script_1");
    assert(eval.is_success);
    assert(eval.value.as_bool() == true);

    // Negative evaluation
    engine.set("score", 70);
    auto eval_false = engine.evaluate("rule_script_1");
    assert(eval_false.is_success);
    assert(eval_false.value.as_bool() == false);

    std::cout << "test_script_parser PASSED!\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "     RUNNING DSL HEXAGONAL DDD TESTS    \n";
    std::cout << "========================================\n";

    test_value_objects();
    test_type_checker();
    test_fluent_edsl();
    test_script_parser();

    std::cout << "========================================\n";
    std::cout << "       ALL TESTS PASSED SUCCESSFULLY!    \n";
    std::cout << "========================================\n";
    return 0;
}
