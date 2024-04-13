#include "catch.hpp"
#include "pkb/Stores/ParentStore.h"
#include "qps/clauseArguments/Integer.h"
#include "qps/clauseArguments/Wildcard.h"

TEST_CASE("ParentStore - All Tests") {
    ParentStore parentStore;

    SECTION("Test getParent") {
        parentStore.setParentStore({{1, 2}, {1, 3}, {1, 4}});
        REQUIRE(parentStore.getParent(2) == 1);
        REQUIRE(parentStore.getParent(3) == 1);
        REQUIRE(parentStore.getParent(4) == 1);
    }

    SECTION("Test getChildren") {
        parentStore.setParentStore({{1, 2}, {1, 3}, {1, 4}});
        REQUIRE(parentStore.getChildren(1) == std::unordered_set<StmtNum>{2, 3, 4});
    }

    SECTION("Test getChildrenStar") {
        parentStore.setParentStore({{1, 2}, {2, 3}, {3, 4}});

        REQUIRE(parentStore.getChildrenStar(1) == std::unordered_set<StmtNum>{2, 3, 4});
        REQUIRE(parentStore.getChildrenStar(2) == std::unordered_set<StmtNum>{3, 4});
        REQUIRE(parentStore.getChildrenStar(3) == std::unordered_set<StmtNum>{4});
        REQUIRE(parentStore.getChildrenStar(4).empty());
    }

    SECTION("Test getParents") {
        parentStore.setParentStore({{1, 2}, {2, 3}, {3, 4}});

        REQUIRE(parentStore.getParent(4) == 3);
        REQUIRE(parentStore.getParent(3) == 2);
        REQUIRE(parentStore.getParent(2) == 1);
    }

    SECTION("Test getParentsStar") {
        parentStore.setParentStore({{1, 2}, {2, 3}, {3, 4}});

        REQUIRE(parentStore.getParentsStar(3) == std::unordered_set<StmtNum>{2, 1});
        REQUIRE(parentStore.getParentsStar(2) == std::unordered_set<StmtNum>{1});
        REQUIRE(parentStore.getParentsStar(1).empty());
    }

    SECTION("Test hasParentStarRelationship") {
        parentStore.setParentStore({{1, 2}, {3, 4}, {5, 6}});

        REQUIRE(parentStore.hasParentRelationship(1, 2));
        REQUIRE(parentStore.hasParentRelationship(3, 4));
        REQUIRE(parentStore.hasParentRelationship(5, 6));

        REQUIRE_FALSE(parentStore.hasParentRelationship(2, 1));
        REQUIRE_FALSE(parentStore.hasParentRelationship(4, 3));
        REQUIRE_FALSE(parentStore.hasParentRelationship(6, 5));
    }
    SECTION("Test hasParentStarRelationship") {
        parentStore.setParentStore({{1, 2}, {3, 4}, {4, 5}, {5, 6}});

        REQUIRE(parentStore.hasParentStarRelationship(1, 2));
        REQUIRE(parentStore.hasParentStarRelationship(3, 5));
        REQUIRE_FALSE(parentStore.hasParentStarRelationship(1, 4));
        REQUIRE_FALSE(parentStore.hasParentStarRelationship(2, 1));
    }

    SECTION("Test hasParentRelationship with Wildcards arg1 and arg2, non-empty parentStore (Expecting True)") {
        parentStore.setParentStore({{1, 2}, {3, 4}, {5, 6}});
        ClauseArgument* wildcardArg1 = new Wildcard();
        ClauseArgument* wildcardArg2 = new Wildcard();
        REQUIRE(parentStore.hasParentRelationship(*wildcardArg1, *wildcardArg2));

        delete wildcardArg1;
        delete wildcardArg2;
    }

    SECTION("Test hasParentRelationship with Wildcards arg1 and arg2, empty parentStore (Expecting False)") {
        parentStore.setParentStore({});
        ClauseArgument* wildcardArg1 = new Wildcard();
        ClauseArgument* wildcardArg2 = new Wildcard();
        REQUIRE_FALSE(parentStore.hasParentRelationship(*wildcardArg1, *wildcardArg2));

        delete wildcardArg1;
        delete wildcardArg2;
    }

    SECTION("Test hasParentRelationship with Wildcard arg1 and Integer arg2 (Expecting True)") {
        parentStore.setParentStore({{1, 2}, {3, 4}, {5, 6}});

        // Argument 1 is wildcard
        // Argument 2 is integer with parent (1)
        ClauseArgument* wildcardArg1 = new Wildcard();
        ClauseArgument* integerArg2 = new Integer("2");
        REQUIRE(parentStore.hasParentRelationship(*wildcardArg1, *integerArg2));

        delete wildcardArg1;
        delete integerArg2;
    }

    SECTION("Test hasParentRelationship with Wildcard arg1 and Integer arg2 (Expecting False)") {
        parentStore.setParentStore({{1, 2}, {3, 4}, {5, 6}});

        // Argument 1 is wildcard
        // Argument 2 is integer with no parent
        ClauseArgument* wildcardArg1 = new Wildcard();
        ClauseArgument* integerArg2 = new Integer("3");
        REQUIRE_FALSE(parentStore.hasParentRelationship(*wildcardArg1, *integerArg2));

        delete wildcardArg1;
        delete integerArg2;
    }

    SECTION("Test hasParentRelationship with Integer arg1 and Wildcard arg2 (Expecting True)") {
        parentStore.setParentStore({{1, 2}, {3, 4}, {5, 6}});

        // Argument 1 is integer and has child (2)
        // Argument 2 is wildcard
        ClauseArgument* integerArg1 = new Integer("1");
        ClauseArgument* wildcardArg2 = new Wildcard();
        REQUIRE(parentStore.hasParentRelationship(*integerArg1, *wildcardArg2));

        delete integerArg1;
        delete wildcardArg2;
    }

    SECTION("Test hasParentRelationship with Integer arg1 and Wildcard arg2 (Expecting false)") {
        parentStore.setParentStore({{1, 2}, {3, 4}, {5, 6}});

        // Test argument 1 is integer, but has no children
        // Test argument 2 is wildcard
        ClauseArgument* integerArg1 = new Integer("6");
        ClauseArgument* wildcardArg2 = new Wildcard();
        REQUIRE_FALSE(parentStore.hasParentRelationship(*integerArg1, *wildcardArg2));

        delete integerArg1;
        delete wildcardArg2;
    }

    SECTION("Test hasParentRelationship with Integer arg1 and Integer arg2 (Expecting true)") {
        parentStore.setParentStore({{1, 2}, {3, 4}, {5, 6}});

        // Test when both arguments are integers and have parent-child relationship
        ClauseArgument* integerArg1 = new Integer("1");
        ClauseArgument* integerArg2 = new Integer("2");
        REQUIRE(parentStore.hasParentRelationship(*integerArg1, *integerArg2));

        delete integerArg1;
        delete integerArg2;
    }

    SECTION("Test hasParentRelationship with Integer arg1 and Integer arg2 (Expecting False)") {
        parentStore.setParentStore({{1, 2}, {3, 4}, {5, 6}});

        // Test when both arguments are integers and have no parent-child relationship
        ClauseArgument* integerArg1 = new Integer("1");
        ClauseArgument* integerArg2 = new Integer("6");
        REQUIRE_FALSE(parentStore.hasParentRelationship(*integerArg1, *integerArg2));

        delete integerArg1;
        delete integerArg2;
    }
}

// Test to ensure no regression from changes to bug fix from TransitiveClosureUtility changes
TEST_CASE("ParentStore - Test for issue #260") {
    ParentStore parentStore;

    // Provided parent pairs based on issue 260
    std::unordered_set<std::pair<StmtNum, StmtNum>> parentPairs = {
        {46, 47}, {40, 41}, {8, 9},   {40, 42}, {33, 35}, {28, 30}, {8, 10},  {33, 34}, {8, 11},  {33, 37},
        {8, 12},  {33, 36}, {8, 13},  {19, 21}, {8, 14},  {33, 38}, {8, 15},  {8, 16},  {30, 39}, {8, 17},
        {27, 28}, {19, 20}, {19, 22}, {19, 45}, {19, 46}, {30, 31}, {28, 29}, {22, 23}, {22, 24}, {22, 25},
        {22, 26}, {22, 27}, {22, 44}, {27, 40}, {27, 43}, {30, 32}, {30, 33}};

    parentStore.setParentStore(parentPairs);

    SECTION("Test getParentsStar") {
        REQUIRE(parentStore.getParentsStar(1).empty());
        REQUIRE(parentStore.getParentsStar(2).empty());
        REQUIRE(parentStore.getParentsStar(3).empty());
        REQUIRE(parentStore.getParentsStar(4).empty());
        REQUIRE(parentStore.getParentsStar(5).empty());
        REQUIRE(parentStore.getParentsStar(6).empty());
        REQUIRE(parentStore.getParentsStar(7).empty());
        REQUIRE(parentStore.getParentsStar(8).empty());
        REQUIRE(parentStore.getParentsStar(9) == std::unordered_set<StmtNum>{8});
        REQUIRE(parentStore.getParentsStar(10) == std::unordered_set<StmtNum>{8});
        REQUIRE(parentStore.getParentsStar(11) == std::unordered_set<StmtNum>{8});
        REQUIRE(parentStore.getParentsStar(12) == std::unordered_set<StmtNum>{8});
        REQUIRE(parentStore.getParentsStar(13) == std::unordered_set<StmtNum>{8});
        REQUIRE(parentStore.getParentsStar(14) == std::unordered_set<StmtNum>{8});
        REQUIRE(parentStore.getParentsStar(15) == std::unordered_set<StmtNum>{8});
        REQUIRE(parentStore.getParentsStar(16) == std::unordered_set<StmtNum>{8});
        REQUIRE(parentStore.getParentsStar(17) == std::unordered_set<StmtNum>{8});
        REQUIRE(parentStore.getParentsStar(18).empty());
        REQUIRE(parentStore.getParentsStar(19).empty());
        REQUIRE(parentStore.getParentsStar(20) == std::unordered_set<StmtNum>{19});
        REQUIRE(parentStore.getParentsStar(21) == std::unordered_set<StmtNum>{19});
        REQUIRE(parentStore.getParentsStar(22) == std::unordered_set<StmtNum>{19});
        REQUIRE(parentStore.getParentsStar(23) == std::unordered_set<StmtNum>{19, 22});
        REQUIRE(parentStore.getParentsStar(24) == std::unordered_set<StmtNum>{19, 22});
        REQUIRE(parentStore.getParentsStar(25) == std::unordered_set<StmtNum>{19, 22});
        REQUIRE(parentStore.getParentsStar(26) == std::unordered_set<StmtNum>{19, 22});
        REQUIRE(parentStore.getParentsStar(27) == std::unordered_set<StmtNum>{19, 22});
        REQUIRE(parentStore.getParentsStar(28) == std::unordered_set<StmtNum>{19, 27, 22});
        REQUIRE(parentStore.getParentsStar(29) == std::unordered_set<StmtNum>{19, 27, 28, 22});
        REQUIRE(parentStore.getParentsStar(30) == std::unordered_set<StmtNum>{19, 27, 28, 22});
        REQUIRE(parentStore.getParentsStar(31) == std::unordered_set<StmtNum>{19, 27, 28, 30, 22});
        REQUIRE(parentStore.getParentsStar(32) == std::unordered_set<StmtNum>{19, 27, 28, 30, 22});
        REQUIRE(parentStore.getParentsStar(33) == std::unordered_set<StmtNum>{19, 27, 28, 30, 22});
        REQUIRE(parentStore.getParentsStar(34) == std::unordered_set<StmtNum>{19, 27, 28, 30, 33, 22});
        REQUIRE(parentStore.getParentsStar(35) == std::unordered_set<StmtNum>{19, 27, 28, 30, 33, 22});
        REQUIRE(parentStore.getParentsStar(36) == std::unordered_set<StmtNum>{19, 27, 28, 30, 33, 22});
        REQUIRE(parentStore.getParentsStar(37) == std::unordered_set<StmtNum>{19, 27, 28, 30, 33, 22});
        REQUIRE(parentStore.getParentsStar(38) == std::unordered_set<StmtNum>{19, 27, 28, 30, 33, 22});
        REQUIRE(parentStore.getParentsStar(39) == std::unordered_set<StmtNum>{19, 27, 28, 30, 22});
        REQUIRE(parentStore.getParentsStar(40) == std::unordered_set<StmtNum>{22, 19, 27});
        REQUIRE(parentStore.getParentsStar(41) == std::unordered_set<StmtNum>{22, 19, 27, 40});
        REQUIRE(parentStore.getParentsStar(42) == std::unordered_set<StmtNum>{22, 19, 27, 40});
        REQUIRE(parentStore.getParentsStar(43) == std::unordered_set<StmtNum>{22, 19, 27});
        REQUIRE(parentStore.getParentsStar(44) == std::unordered_set<StmtNum>{22, 19});
        REQUIRE(parentStore.getParentsStar(45) == std::unordered_set<StmtNum>{19});
        REQUIRE(parentStore.getParentsStar(46) == std::unordered_set<StmtNum>{19});
        REQUIRE(parentStore.getParentsStar(47) == std::unordered_set<StmtNum>{46, 19});
        REQUIRE(parentStore.getParentsStar(48).empty());
    }

    SECTION("Test getChildrenStar") {
        REQUIRE(parentStore.getChildrenStar(1).empty());
        REQUIRE(parentStore.getChildrenStar(2).empty());
        REQUIRE(parentStore.getChildrenStar(3).empty());
        REQUIRE(parentStore.getChildrenStar(4).empty());
        REQUIRE(parentStore.getChildrenStar(5).empty());
        REQUIRE(parentStore.getChildrenStar(6).empty());
        REQUIRE(parentStore.getChildrenStar(7).empty());
        REQUIRE(parentStore.getChildrenStar(8) == std::unordered_set<StmtNum>{9, 10, 11, 12, 13, 14, 15, 16, 17});
        REQUIRE(parentStore.getChildrenStar(9).empty());
        REQUIRE(parentStore.getChildrenStar(10).empty());
        REQUIRE(parentStore.getChildrenStar(11).empty());
        REQUIRE(parentStore.getChildrenStar(12).empty());
        REQUIRE(parentStore.getChildrenStar(13).empty());
        REQUIRE(parentStore.getChildrenStar(14).empty());
        REQUIRE(parentStore.getChildrenStar(15).empty());
        REQUIRE(parentStore.getChildrenStar(16).empty());
        REQUIRE(parentStore.getChildrenStar(17).empty());
        REQUIRE(parentStore.getChildrenStar(18).empty());
        REQUIRE(parentStore.getChildrenStar(19) == std::unordered_set<StmtNum>{20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                                                                               30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                                                                               40, 41, 42, 43, 44, 45, 46, 47});
        REQUIRE(parentStore.getChildrenStar(20).empty());
        REQUIRE(parentStore.getChildrenStar(21).empty());
        REQUIRE(parentStore.getChildrenStar(22) == std::unordered_set<StmtNum>{23, 32, 24, 25, 26, 27, 28, 44,
                                                                               40, 43, 30, 29, 31, 39, 33, 35,
                                                                               34, 37, 36, 38, 41, 42});
        REQUIRE(parentStore.getChildrenStar(23).empty());
        REQUIRE(parentStore.getChildrenStar(24).empty());
        REQUIRE(parentStore.getChildrenStar(25).empty());
        REQUIRE(parentStore.getChildrenStar(26).empty());
        REQUIRE(parentStore.getChildrenStar(27) ==
                std::unordered_set<StmtNum>{28, 32, 40, 43, 30, 29, 31, 39, 33, 35, 34, 37, 36, 38, 41, 42});
        REQUIRE(parentStore.getChildrenStar(28) ==
                std::unordered_set<StmtNum>{30, 29, 39, 31, 32, 33, 35, 34, 37, 36, 38});
        REQUIRE(parentStore.getChildrenStar(29).empty());
        REQUIRE(parentStore.getChildrenStar(30) == std::unordered_set<StmtNum>{31, 39, 32, 33, 35, 34, 37, 36, 38});
        REQUIRE(parentStore.getChildrenStar(31).empty());
        REQUIRE(parentStore.getChildrenStar(32).empty());
        REQUIRE(parentStore.getChildrenStar(33) == std::unordered_set<StmtNum>{35, 34, 37, 36, 38});
        REQUIRE(parentStore.getChildrenStar(34).empty());
        REQUIRE(parentStore.getChildrenStar(35).empty());
        REQUIRE(parentStore.getChildrenStar(36).empty());
        REQUIRE(parentStore.getChildrenStar(37).empty());
        REQUIRE(parentStore.getChildrenStar(38).empty());
        REQUIRE(parentStore.getChildrenStar(39).empty());
        REQUIRE(parentStore.getChildrenStar(40) == std::unordered_set<StmtNum>{41, 42});
        REQUIRE(parentStore.getChildrenStar(41).empty());
        REQUIRE(parentStore.getChildrenStar(42).empty());
        REQUIRE(parentStore.getChildrenStar(43).empty());
        REQUIRE(parentStore.getChildrenStar(44).empty());
        REQUIRE(parentStore.getChildrenStar(45).empty());
        REQUIRE(parentStore.getChildrenStar(46) == std::unordered_set<StmtNum>{47});
        REQUIRE(parentStore.getChildrenStar(47).empty());
        REQUIRE(parentStore.getChildrenStar(48).empty());
    }
}
