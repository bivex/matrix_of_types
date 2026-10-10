# ⬡ C++ DSL Library (Hexagonal Architecture + DDD)

A modern C++20 Domain-Specific Language (DSL) Rule & Expression Engine designed according to **Hexagonal Architecture (Ports & Adapters)** and **Domain-Driven Design (DDD)** principles.

---

## 🏛️ Architectural Overview

```
                 +-------------------------------------------------------------+
                 |                     HEXAGON / BOUNDARY                      |
                 |                                                             |
[Inbound Adapter]| ===> [Inbound Port]  +--------------------+  [Outbound Port] | ===> [Outbound Adapter]
(C++ Fluent EDSL)|      (CompileRule)   | APPLICATION LAYER  |  (ContextRepo)  |      (InMemoryContext)
                 |                      | - CompileUseCase   |                 |
[Inbound Adapter]| ===> [Inbound Port]  | - EvaluateUseCase  |  [Outbound Port] | ===> [Outbound Adapter]
(Script Parser)  |      (EvaluateRule)  +--------------------+  (RuleRepo)     |      (InMemoryRuleRepo)
                 |                                |                            |
                 |                                v                            |
                 |                     +--------------------+   [Outbound Port] | ===> [Outbound Adapter]
                 |                     |    DOMAIN CORE     |   (Logger)       |      (ConsoleLogger)
                 |                     | - ExpressionAgg    |                  |
                 |                     | - Value Objects    |                  |
                 |                     | - AST Hierarchy    |                  |
                 |                     | - EvaluatorService |                  |
                 |                     | - TypeCheckService |                  |
                 |                     +--------------------+                  |
                 +-------------------------------------------------------------+
```

---

## 📂 Project Structure

```
lib_test/
├── CMakeLists.txt
├── README.md
├── include/
│   └── dsl/
│       ├── dsl.hpp                                 # Unified engine facade
│       ├── domain/                                 # PURE DOMAIN LAYER (Zero external dependencies)
│       │   ├── model/
│       │   │   ├── types.hpp                       # Value Objects: Literal, Identifier, DslType, OperatorType
│       │   │   ├── ast_nodes.hpp                   # AST Node Hierarchy: BinaryNode, UnaryNode, LiteralNode, VariableNode
│       │   │   └── expression_aggregate.hpp        # Aggregate Root: ExpressionAggregate (enforces invariants & events)
│       │   ├── events/
│       │   │   └── domain_events.hpp               # Domain Events: RuleCompiledEvent, RuleEvaluatedEvent
│       │   ├── services/
│       │   │   ├── evaluator_service.hpp           # Visitor-based AST Evaluator Service
│       │   │   └── type_checker_service.hpp        # Compile-time Type Validation Service
│       │   └── ports/                              # BOUNDARIES (Hexagonal Interfaces)
│       │       ├── inbound/                        # Primary / Driving Ports
│       │       │   ├── compile_rule_port.hpp
│       │       │   └── evaluate_rule_port.hpp
│       │       └── outbound/                       # Secondary / Driven Ports
│       │           ├── context_repository_port.hpp
│       │           ├── rule_repository_port.hpp
│       │           └── logger_port.hpp
│       ├── application/                            # USE CASES ORCHESTRATION
│       │   ├── dtos/
│       │   │   └── rule_dto.hpp                    # Commands and Result DTOs
│       │   └── use_cases/
│       │       ├── compile_rule_use_case.hpp       # Implements ICompileRulePort
│       │       └── evaluate_rule_use_case.hpp      # Implements IEvaluateRulePort
│       └── adapters/                               # INFRASTRUCTURE & PRESENTATION ADAPTERS
│           ├── inbound/                            # Driving Adapters
│           │   ├── fluent_edsl_adapter.hpp         # C++ Embedded DSL with Operator Overloading
│           │   └── script_parser_adapter.hpp       # Recursive-Descent External Text DSL Parser
│           └── outbound/                           # Driven Adapters
│               ├── in_memory_context_adapter.hpp   # Context Repository Adapter (Thread-Safe)
│               ├── in_memory_rule_repository.hpp   # Rule Storage Adapter (Thread-Safe)
│               └── console_logger_adapter.hpp      # Diagnostics Logger Adapter
├── tests/
│   └── test_dsl.cpp                                # Automated Unit Test Suite
└── examples/
    └── main.cpp                                    # End-to-End Runnable Demo
```

---

## 🚀 Key Features

### 1. Inbound Driving Adapter #1: Modern C++ Fluent Embedded DSL (EDSL)
Construct and compose expressions directly in C++ using natural operator overloading and expression templates:

```cpp
#include "dsl/dsl.hpp"
using namespace dsl::adapters::inbound;

auto discount_rule = rule("promo_10", "10% Discount Promotion")
    .when(
        (var("age") >= 21 && var("tier") == "GOLD") &&
        (var("spend") * 1.15 > 1500.0) &&
        !var("is_restricted")
    );
```

### 2. Inbound Driving Adapter #2: External Script Parser (Text DSL)
Parse external script strings safely into the Domain AST with recursive descent parsing:

```cpp
std::string script = "(score >= 80 && department == 'IT') || is_admin == true";
engine.compile_script("rule_script_1", "Access Rule", script);
```

### 3. Domain Model & DDD Patterns
* **Aggregate Root (`ExpressionAggregate`)**: Enforces business invariants, ensures AST integrity, and records domain events (`RuleCompiledEvent`, `RuleEvaluatedEvent`).
* **Value Objects**:
  * `Literal`: Type-safe variant holding `int64_t`, `double`, `bool`, `std::string`, or `null`.
  * `Identifier`: Validated variable name with alphanumeric rules.
  * `OperatorType`: Typed enumeration of arithmetic, relational, and logical operations.
* **Domain Services**:
  * `TypeCheckerService`: Validates semantic correctness of AST trees before compilation (rejecting operations like `"string" && 42`).
  * `ExpressionEvaluatorService`: Visitor-based pure evaluation against outbound context ports.

### 4. Hexagonal Decoupling (Ports & Adapters)
* All core domain and application logic depend **only on interfaces (Ports)**.
* Repositories, parsers, loggers, and context providers can be swapped (e.g. database-backed context, JSON logger, Redis rule storage) without altering a single line of domain logic.

---

## 🛠️ Building & Running

Requires a modern C++20 compiler (`AppleClang 15+`, `GCC 11+`, or `Clang 14+`) and CMake 3.20+.

```bash
cd lib_test
cmake -B build -S .
cmake --build build
```

### Run Unit Tests
```bash
./build/dsl_tests
```

### Run Demo
```bash
./build/dsl_demo
```
