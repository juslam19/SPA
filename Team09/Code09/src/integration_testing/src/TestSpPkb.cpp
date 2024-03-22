#include <filesystem>
#include <unordered_set>

#include "PKB/PKB.h"
#include "PKB/Utils/DataTypes.h"
#include "Spa.h"
#include "catch.hpp"
#include "qps/clauseArguments/Integer.h"
#include "qps/clauseArguments/Literal.h"

const char CODE_DIRECTORY_NAME[] = "Code09";

TEST_CASE("SP-PKB: Design Entities stored correctly") {
    SPA spa;
    auto filePath = std::filesystem::current_path();
    while (filePath.filename() != CODE_DIRECTORY_NAME) {
        filePath = filePath.parent_path();
    }

    filePath /= "src/integration_testing/test_files/basic_source.txt";

    spa.parse(filePath.string());
    PKB& pkb = spa.getInternalPKB();
    PKBFacadeReader pkbFacadeReader(pkb);

    SECTION("Variables stored correctly") {
        std::unordered_set<Variable> variableSet = pkbFacadeReader.getVariables();
        std::unordered_set<Variable> expectedVariableSet = {
            "a",   "c",   "d",   "e",   "f",   "g",   "h",    "i",     "abc",  "xyz", "q",     "w",  "y",   "z",
            "r",   "t",   "a0",  "a1",  "a2",  "a3",  "a4",   "a5",    "a6",   "a7",  "a8",    "a9", "a10", "a14",
            "a15", "a16", "a17", "a20", "a21", "a22", "a23",  "a25",   "a33",  "a34", "while", "if", "gg",  "hh",
            "ii",  "o",   "m",   "x",   "b",   "b1",  "then", "print", "else", "a34", "read"};
        REQUIRE(variableSet == expectedVariableSet);
    }

    SECTION("Constants stored correctly") {
        std::unordered_set<Constant> constantsSet = pkbFacadeReader.getConstants();
        std::unordered_set<Constant> expectedConstantsSet = {"2",  "4",   "999", "1111", "9999", "50", "110", "40",
                                                             "10", "170", "29",  "1",    "11",   "90", "22",  "55",
                                                             "18", "3",   "34",  "27",   "15",   "6",  "68",  "0"};
        REQUIRE(constantsSet == expectedConstantsSet);
    }

    SECTION("Statements stored correctly") {
        std::unordered_set<Stmt> stmtSet = pkbFacadeReader.getStmts();
        std::unordered_set<Stmt> expectedStmtSet = {};
        std::unordered_set<int> assign = {1, 2, 3, 4, 5, 8, 9, 12, 13, 14, 15, 16, 17, 27, 28, 31, 32, 37, 39, 42};
        std::unordered_set<int> print = {6, 7, 10, 11, 20, 24, 35, 43, 47};
        std::unordered_set<int> read = {18, 25, 44, 48};
        std::unordered_set<int> ifs = {19, 22, 23, 26, 30, 34};
        std::unordered_set<int> call = {21, 29, 33, 36};
        std::unordered_set<int> whiles = {38, 40, 41, 45, 46};
        for (int i = 1; i < 51; i++) {
            if (assign.find(i) != assign.end()) {
                expectedStmtSet.insert({StatementType::ASSIGN, i});
            } else if (print.find(i) != print.end()) {
                expectedStmtSet.insert({StatementType::PRINT, i});
            } else if (read.find(i) != read.end()) {
                expectedStmtSet.insert({StatementType::READ, i});
            } else if (ifs.find(i) != ifs.end()) {
                expectedStmtSet.insert({StatementType::IF, i});
            } else if (call.find(i) != call.end()) {
                expectedStmtSet.insert({StatementType::CALL, i});
            } else if (whiles.find(i) != whiles.end()) {
                expectedStmtSet.insert({StatementType::WHILE, i});
            }
        }
        REQUIRE(stmtSet == expectedStmtSet);
    }

    SECTION("Procedures stored correctly") {
        std::unordered_set<Procedure> procSet = pkbFacadeReader.getProcedures();
        std::unordered_set<Procedure> expectedProcSet = {"main", "if", "while"};
        REQUIRE(procSet == expectedProcSet);
    }
}

TEST_CASE("SP-PKB: Design Abstractions stored correctly") {
    SPA spa;
    auto filePath = std::filesystem::current_path();
    while (filePath.filename() != CODE_DIRECTORY_NAME) {
        filePath = filePath.parent_path();
    }

    filePath /= "src/integration_testing/test_files/basic_source.txt";

    spa.parse(filePath.string());
    PKB& pkb = spa.getInternalPKB();
    PKBFacadeReader pkbFacadeReader(pkb);

    SECTION("Follows relationship stored correctly") {
        REQUIRE(pkbFacadeReader.hasFollowRelationship(5, 6));
        REQUIRE(pkbFacadeReader.hasFollowRelationship(6, 7));
        REQUIRE(pkbFacadeReader.hasFollowRelationship(7, 8));
        REQUIRE_FALSE(pkbFacadeReader.hasFollowRelationship(18, 19));
        REQUIRE_FALSE(pkbFacadeReader.hasFollowRelationship(20, 21));
        REQUIRE(pkbFacadeReader.hasFollowRelationship(19, 22));
        REQUIRE_FALSE(pkbFacadeReader.hasFollowRelationship(22, 23));
        REQUIRE(pkbFacadeReader.hasFollowRelationship(25, 26));
        REQUIRE(pkbFacadeReader.hasFollowRelationship(26, 30));
        REQUIRE(pkbFacadeReader.hasFollowRelationship(38, 40));
        REQUIRE(pkbFacadeReader.hasFollowRelationship(41, 45));
    }

    SECTION("Parent relationship stored correctly") {
        REQUIRE(pkbFacadeReader.hasParentRelationship(19, 20));
        REQUIRE(pkbFacadeReader.hasParentRelationship(22, 23));
        REQUIRE(pkbFacadeReader.hasParentRelationship(22, 23));
        REQUIRE(pkbFacadeReader.hasParentRelationship(23, 26));
        REQUIRE(pkbFacadeReader.hasParentRelationship(23, 30));
        REQUIRE(pkbFacadeReader.hasParentRelationship(38, 39));
        REQUIRE(pkbFacadeReader.hasParentRelationship(40, 41));
        REQUIRE(pkbFacadeReader.hasParentRelationship(40, 45));
    }

    SECTION("Uses relationship stored correctly - statements") {
        auto assignStmtNum = Integer("2");
        auto variable = Literal("xyz");
        REQUIRE(pkbFacadeReader.hasStatementVariableUseRelationship(assignStmtNum, variable));
        auto pn = Integer("6");
        auto variablePrint = Literal("a");
        REQUIRE(pkbFacadeReader.hasStatementVariableUseRelationship(pn, variablePrint));
        auto nestedIf = Integer("23");
        auto variableIf = Literal("if");
        REQUIRE(pkbFacadeReader.hasStatementVariableUseRelationship(nestedIf, variableIf));
        auto nestedWhile = Integer("41");
        auto variableZ = Literal("z");
        REQUIRE(pkbFacadeReader.hasStatementVariableUseRelationship(nestedWhile, variableZ));
        // auto call = Integer("36");
        // auto procName = Literal("main");
        // REQUIRE(pkbFacadeReader.hasStatementVariableUseRelationship(call, procName));
    }

    SECTION("Uses relationship stored correctly - procedures") {
        auto main = Literal("main");
        auto yLiteral = Literal("y");
        REQUIRE(pkbFacadeReader.hasProcedureVariableUseRelationship(main, yLiteral));
        auto ifProc = Literal("if");
        auto print = Literal("print");
        auto then = Literal("then");
        REQUIRE(pkbFacadeReader.hasProcedureVariableUseRelationship(ifProc, print));
        REQUIRE(pkbFacadeReader.hasProcedureVariableUseRelationship(ifProc, then));
        auto whileProc = Literal("while");
        auto a0 = Literal("a0");
        auto c = Literal("c");
        REQUIRE(pkbFacadeReader.hasProcedureVariableUseRelationship(whileProc, a0));
        REQUIRE(pkbFacadeReader.hasProcedureVariableUseRelationship(whileProc, c));
    }

    SECTION("Modifies relationship stored correctly - statements") {
        auto print = Literal("print");
        auto printLnNumber = Integer("31");
        REQUIRE(pkbFacadeReader.hasStatementVariableModifiesRelationship(printLnNumber, print));
        auto read = Literal("read");
        auto readLineNumber = Integer("25");
        REQUIRE(pkbFacadeReader.hasStatementVariableModifiesRelationship(readLineNumber, read));
    }

    SECTION("Modifies relationship stored correctly - procedures") {
        auto mainProc = Literal("main");
        auto variableA10 = Literal("a10");
        REQUIRE(pkbFacadeReader.hasProcedureVariableModifiesRelationship(mainProc, variableA10));
        auto ifProc = Literal("if");
        auto print = Literal("print");
        auto read = Literal("read");
        REQUIRE(pkbFacadeReader.hasProcedureVariableModifiesRelationship(ifProc, print));
        REQUIRE(pkbFacadeReader.hasProcedureVariableModifiesRelationship(ifProc, read));
        auto whileProc = Literal("while");
        auto a21 = Literal("a21");
        auto a1 = Literal("a1");
        REQUIRE(pkbFacadeReader.hasProcedureVariableModifiesRelationship(whileProc, a21));
        REQUIRE(pkbFacadeReader.hasProcedureVariableModifiesRelationship(whileProc, a1));
    }

    SECTION("Calls relationship stored correctly") {
        auto ifProc = Literal("if");
        auto main = Literal("main");
        auto whileProc = Literal("while");
        REQUIRE(pkbFacadeReader.hasCallRelationship(ifProc, main));
        REQUIRE(pkbFacadeReader.hasCallRelationship(ifProc, whileProc));
    }

    SECTION("Follows relationship stored correctly") {
        REQUIRE_FALSE(pkbFacadeReader.hasFollowRelationship(18, 19));
        REQUIRE(pkbFacadeReader.hasFollowRelationship(19, 22));
        REQUIRE(pkbFacadeReader.hasFollowRelationship(26, 30));
        REQUIRE(pkbFacadeReader.hasFollowRelationship(41, 45));
    }

    SECTION("Next relationship stored correctly") {
        REQUIRE_FALSE(pkbFacadeReader.hasNextRelationship(18, 19));
        REQUIRE(pkbFacadeReader.hasNextRelationship(19, 20));
        REQUIRE(pkbFacadeReader.hasNextRelationship(19, 21));
        REQUIRE(pkbFacadeReader.hasNextRelationship(20, 22));
        REQUIRE(pkbFacadeReader.hasNextRelationship(21, 22));
        REQUIRE(pkbFacadeReader.hasNextRelationship(22, 23));
        REQUIRE(pkbFacadeReader.hasNextRelationship(22, 34));
        REQUIRE(pkbFacadeReader.hasNextRelationship(27, 28));
        REQUIRE(pkbFacadeReader.hasNextRelationship(28, 30));
        REQUIRE(pkbFacadeReader.hasNextRelationship(29, 30));
        REQUIRE(pkbFacadeReader.hasNextRelationship(34, 36));
        REQUIRE(pkbFacadeReader.hasNextRelationship(34, 35));
        REQUIRE(pkbFacadeReader.hasNextRelationship(35, 37));
    }

    SECTION("Pattern stored correctly") {}
}
