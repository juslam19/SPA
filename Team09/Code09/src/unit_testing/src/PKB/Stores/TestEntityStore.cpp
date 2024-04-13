#include "catch.hpp"
#include "pkb/Stores/EntityStore.h"

// ai-gen start(gpt, 1, e)
// prompt:
// https://platform.openai.com/playground/p/yzre7t43UnEzHeaMn64z5jkq?model=gpt-4&mode=chat
using Procedure = std::string;
using Variable = std::string;
using Constant = std::string;

TEST_CASE("EntityStore - Base Test") {
    EntityStore<Procedure> procedureStore;
    std::unordered_set<Procedure> procedures = {"yishun", "bedok", "main"};

    SECTION("Test addEntities and getAllEntities") {
        procedureStore.addEntities(procedures);

        REQUIRE(procedureStore.getAllEntities() == procedures);
    }

    EntityStore<Variable> variableStore;
    std::unordered_set<Variable> variables = {"a", "b", "c"};

    SECTION("Test addEntities and hasEntity") {
        variableStore.addEntities(variables);

        REQUIRE(variableStore.hasEntity("a"));
        REQUIRE(variableStore.hasEntity("b"));
        REQUIRE(variableStore.hasEntity("c"));
        // Test for non-existent variable
        REQUIRE_FALSE(variableStore.hasEntity("variable4"));
    }

    EntityStore<Constant> constantStore;
    std::unordered_set<Constant> constants = {"1", "2", "3"};

    SECTION("Test addEntities and hasEntity") {
        constantStore.addEntities(constants);

        REQUIRE(constantStore.hasEntity("1"));
        REQUIRE(constantStore.hasEntity("2"));
        REQUIRE(constantStore.hasEntity("3"));
        // Test for non-existent variable
        REQUIRE_FALSE(constantStore.hasEntity("constant4"));
    }

    EntityStore<Variable> emptyStore;

    SECTION("Empty EntityStore Test") {
        REQUIRE(emptyStore.getAllEntities().empty());
        // Test hasEntity on an empty store
        REQUIRE_FALSE(emptyStore.hasEntity("randomEntity"));
    }
}
// ai-gen end
