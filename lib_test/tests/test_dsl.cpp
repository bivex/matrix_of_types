#include "dsl/dsl.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

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
    assert(eval2.value.as_bool() == false);

    std::cout << "test_fluent_edsl PASSED!\n";
}

void test_all_math_operations() {
    std::cout << "--- Running test_all_math_operations ---\n";
    DslEngine engine;

    engine.set("x", 9.0)
          .set("y", 16.0)
          .set("angle", 0.0)
          .set("val", -42.7)
          .set("a_int", 24)
          .set("b_int", 36);

    // 1. Roots and hypotenuse: hypot(x, y) == sqrt(x^2 + y^2)
    auto rule_hypot = rule("rule_hypot", "Hypotenuse check")
        .when(hypot(var("x"), var("y")) == sqrt((var("x") ^ 2) + (var("y") ^ 2)));
    assert(engine.compile(rule_hypot).is_success);
    auto res_hypot = engine.evaluate("rule_hypot");
    assert(res_hypot.is_success && res_hypot.value.as_bool() == true);

    // 2. Trigonometry & Constants: sin(0) == 0, cos(0) == 1
    auto rule_trig = rule("rule_trig", "Trig check")
        .when(sin(var("angle")) == 0.0 && cos(var("angle")) == 1.0);
    assert(engine.compile(rule_trig).is_success);
    auto res_trig = engine.evaluate("rule_trig");
    assert(res_trig.is_success && res_trig.value.as_bool() == true);

    // 3. Logarithms and Exponents: log(exp(1)) == 1
    auto rule_exp = rule("rule_exp", "Exp/Log check")
        .when(round(log(exp(lit(1.0)))) == 1.0);
    assert(engine.compile(rule_exp).is_success);
    auto res_exp = engine.evaluate("rule_exp");
    assert(res_exp.is_success && res_exp.value.as_bool() == true);

    // 4. Abs, Floor, Ceil, Clamp
    auto rule_rounding = rule("rule_rounding", "Rounding check")
        .when(abs(var("val")) > 42.0 && floor(var("val")) == -43.0 && clamp(150, 0, 100) == 100);
    assert(engine.compile(rule_rounding).is_success);
    auto res_rounding = engine.evaluate("rule_rounding");
    assert(res_rounding.is_success && res_rounding.value.as_bool() == true);

    // 5. Discrete Math: gcd(24, 36) == 12, lcm(24, 36) == 72, factorial(5) == 120
    auto rule_discrete = rule("rule_discrete", "Discrete math check")
        .when(gcd(var("a_int"), var("b_int")) == 12 &&
              lcm(var("a_int"), var("b_int")) == 72 &&
              factorial(5) == 120);
    assert(engine.compile(rule_discrete).is_success);
    auto res_discrete = engine.evaluate("rule_discrete");
    assert(res_discrete.is_success && res_discrete.value.as_bool() == true);

    // 6. Bitwise operations: (a_int & 15) == 8, (1 << 4) == 16
    auto rule_bitwise = rule("rule_bitwise", "Bitwise check")
        .when((var("a_int") & 15) == 8 && (lit(1) << 4) == 16);
    assert(engine.compile(rule_bitwise).is_success);
    auto res_bitwise = engine.evaluate("rule_bitwise");
    assert(res_bitwise.is_success && res_bitwise.value.as_bool() == true);

    std::cout << "test_all_math_operations PASSED!\n";
}

void test_script_parser() {
    std::cout << "--- Running test_script_parser ---\n";
    DslEngine engine;

    engine.set("score", 85)
          .set("is_admin", false)
          .set("department", std::string("IT"))
          .set("radius", 5.0);

    // 1. Text script logic
    std::string script1 = "(score >= 80 && department == 'IT') || is_admin == true";
    auto comp1 = engine.compile_script("rule_script_1", "Text Script Test", script1);
    assert(comp1.is_success);
    auto eval1 = engine.evaluate("rule_script_1");
    assert(eval1.is_success && eval1.value.as_bool() == true);

    // 2. Text script math functions: circle area = pi * r^2
    std::string script_math = "round(pi * radius ^ 2) == 79"; // pi * 25 ≈ 78.5398 -> round = 79
    auto comp_math = engine.compile_script("rule_circle_area", "Area Math Test", script_math);
    assert(comp_math.is_success);
    auto eval_math = engine.evaluate("rule_circle_area");
    assert(eval_math.is_success && eval_math.value.as_bool() == true);

    // 3. Text script nested functions: clamp(sqrt(100), 0, 5) == 5
    std::string script_nested = "clamp(sqrt(100), 0, 5) == 5 && min(10, 20) == 10 && max(3, 7) == 7";
    auto comp_nested = engine.compile_script("rule_nested_math", "Nested Math Test", script_nested);
    assert(comp_nested.is_success);
    auto eval_nested = engine.evaluate("rule_nested_math");
    assert(eval_nested.is_success && eval_nested.value.as_bool() == true);

    std::cout << "test_script_parser PASSED!\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "     RUNNING DSL HEXAGONAL DDD TESTS    \n";
    std::cout << "========================================\n";

    test_value_objects();
    test_type_checker();
    test_fluent_edsl();
    test_all_math_operations();
    test_script_parser();

    std::cout << "========================================\n";
    std::cout << "       ALL TESTS PASSED SUCCESSFULLY!    \n";
    std::cout << "========================================\n";
    return 0;
}
