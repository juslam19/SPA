#include <memory>

#include "catch.hpp"
#include "sp/de/DesignExtractor.h"

TEST_CASE("Design Extractor Tests") {
    /*
    01  x = y;
    02  read a;
    03  print z;
    04  while ((c < d) && (e > f)) {
    05      g = h;
        }
    06  if (( i < j ) && ( k > l )) then {
    07      i = j;
        } else {
    08      a = b;
        }
    09 a = 1;
    */

    // x = y;
    std::shared_ptr<VariableNode> xNode = std::make_shared<VariableNode>(VariableNode("x", 1));
    std::shared_ptr<VariableNode> yNode = std::make_shared<VariableNode>(VariableNode("y", 1));
    auto equalNode = std::make_shared<AssignmentNode>(xNode, yNode, 1);

    // read a;
    std::shared_ptr<VariableNode> aNode = std::make_shared<VariableNode>(VariableNode("a", 2));
    auto readNode = std::make_shared<ReadNode>(aNode, 2);

    // print z;
    std::shared_ptr<VariableNode> zNode = std::make_shared<VariableNode>(VariableNode("z", 3));
    auto printNode = std::make_shared<PrintNode>(zNode, 3);

    // while ((c < d) && (e > f)) {
    //     g = h;
    // }
    std::shared_ptr<VariableNode> cNode = std::make_shared<VariableNode>(VariableNode("c", 4));
    std::shared_ptr<VariableNode> dNode = std::make_shared<VariableNode>(VariableNode("d", 4));
    std::shared_ptr<VariableNode> eNode = std::make_shared<VariableNode>(VariableNode("e", 4));
    std::shared_ptr<VariableNode> fNode = std::make_shared<VariableNode>(VariableNode("f", 4));
    std::shared_ptr<VariableNode> gNode = std::make_shared<VariableNode>(VariableNode("g", 5));
    std::shared_ptr<VariableNode> hNode = std::make_shared<VariableNode>(VariableNode("h", 5));

    std::vector<std::shared_ptr<StatementNode>> childrenW6 = {};

    auto WlessThan = std::make_shared<ExpressionNode>(LEXICAL_TOKEN_TYPE::LESS_THAN, cNode, dNode, 4);

    auto WmoreThan = std::make_shared<ExpressionNode>(LEXICAL_TOKEN_TYPE::GREATER_THAN, eNode, fNode, 4);

    auto WandandNode = std::make_shared<ExpressionNode>(LEXICAL_TOKEN_TYPE::ANDAND, WlessThan, WmoreThan, 4);

    auto assign = std::make_shared<AssignmentNode>(gNode, hNode, 5);
    childrenW6.push_back(assign);
    auto WstmtList = std::make_shared<StatementListNode>(childrenW6);

    auto whileNode = std::make_shared<WhileNode>(WandandNode, WstmtList, 4);

    // if (( i < j ) && ( k > l )) then {
    //       i = j;
    // } else {
    //       a = b;
    // }

    std::shared_ptr<VariableNode> iNode = std::make_shared<VariableNode>(VariableNode("i", 6));
    std::shared_ptr<VariableNode> jNode = std::make_shared<VariableNode>(VariableNode("j", 6));
    std::shared_ptr<VariableNode> kNode = std::make_shared<VariableNode>(VariableNode("k", 6));
    std::shared_ptr<VariableNode> lNode = std::make_shared<VariableNode>(VariableNode("l", 6));
    std::shared_ptr<VariableNode> iNode1 = std::make_shared<VariableNode>(VariableNode("i", 7));
    std::shared_ptr<VariableNode> jNode1 = std::make_shared<VariableNode>(VariableNode("j", 7));
    std::shared_ptr<VariableNode> aNode1 = std::make_shared<VariableNode>(VariableNode("a", 8));
    std::shared_ptr<VariableNode> bNode1 = std::make_shared<VariableNode>(VariableNode("b", 8));

    std::vector<std::shared_ptr<StatementNode>> childrenI5 = {};
    std::vector<std::shared_ptr<StatementNode>> childrenI7 = {};

    auto IlessThan = std::make_shared<ExpressionNode>(LEXICAL_TOKEN_TYPE::LESS_THAN, iNode, jNode, 6);

    auto ImoreThan = std::make_shared<ExpressionNode>(LEXICAL_TOKEN_TYPE::GREATER_THAN, kNode, lNode, 6);

    auto IandandNode = std::make_shared<ExpressionNode>(LEXICAL_TOKEN_TYPE::ANDAND, IlessThan, ImoreThan, 6);

    auto Iassign = std::make_shared<AssignmentNode>(iNode1, jNode1, 7);
    childrenI5.push_back(Iassign);
    auto IstmtList = std::make_shared<StatementListNode>(childrenI5);

    auto Iassign1 = std::make_shared<AssignmentNode>(aNode1, bNode1, 8);
    childrenI7.push_back(Iassign1);
    auto IstmtList2 = std::make_shared<StatementListNode>(childrenI7);

    auto ifNode = std::make_shared<IfNode>(IandandNode, IstmtList, IstmtList2, 6);

    // x = 1;

    std::shared_ptr<VariableNode> xNode2 = std::make_shared<VariableNode>(VariableNode("x", 9));
    std::shared_ptr<ConstantNode> constNode = std::make_shared<ConstantNode>(ConstantNode("1", 9));

    auto xAssign = std::make_shared<AssignmentNode>(xNode2, constNode, 9);

    std::vector<std::shared_ptr<StatementNode>> childrenProc = {};
    childrenProc.push_back(equalNode);
    childrenProc.push_back(readNode);
    childrenProc.push_back(printNode);
    childrenProc.push_back(whileNode);
    childrenProc.push_back(ifNode);
    childrenProc.push_back(xAssign);

    auto stmtListProc = std::make_shared<StatementListNode>(childrenProc);
    auto ProcNode = std::make_shared<ProcedureNode>("main", stmtListProc);
    std::vector<std::shared_ptr<ProcedureNode>> childrenProg = {};
    childrenProg.push_back(ProcNode);

    auto ProgNode = std::make_shared<ProgramNode>(childrenProg);

    /*
   procedure test {
   11 read a;
   }
    */
    auto variableA = std::make_shared<VariableNode>("a", 11);
    auto readA = std::make_shared<ReadNode>(variableA, 11);
    std::vector<std::shared_ptr<StatementNode>> children2 = {};
    children2.push_back(readA);
    auto stmtListProc1 = std::make_shared<StatementListNode>(children2);

    auto ProcNode2 = std::make_shared<ProcedureNode>("test", stmtListProc1);
    std::vector<std::shared_ptr<ProcedureNode>> childrenProg2 = {};

    childrenProg2.push_back(ProcNode2);
    auto ProgNode2 = std::make_shared<ProgramNode>(childrenProg2);

    std::vector<std::shared_ptr<ProcedureNode>> childrenProg3 = {};

    childrenProg3.push_back(ProcNode);
    childrenProg3.push_back(ProcNode2);

    auto ProgNode3 = std::make_shared<ProgramNode>(childrenProg3);

    SECTION("Variables extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode);
        std::unordered_set<std::string> expectedVariables = {"x", "y", "a", "z", "c", "d", "e", "f",
                                                             "g", "h", "i", "j", "k", "l", "b"};
        REQUIRE(expectedVariables == designExtractor->getVariables());
    }

    SECTION("Single line procedure has variables extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode2);
        std::unordered_set<std::string> expectedVariables = {"a"};
        REQUIRE(expectedVariables == designExtractor->getVariables());
    }

    SECTION("Multiple procedures variables extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode3);
        std::unordered_set<std::string> expectedVariables = {"x", "y", "a", "z", "c", "d", "e", "f",
                                                             "g", "h", "i", "j", "k", "l", "b"};
        REQUIRE(expectedVariables == designExtractor->getVariables());
    }

    SECTION("Constants extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode);
        std::unordered_set<std::string> expectedConstants = {"1"};
        REQUIRE(expectedConstants == designExtractor->getConstants());
    }

    SECTION("Single line procedure has constants extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode2);
        std::unordered_set<std::string> expectedConstants = {};
        REQUIRE(expectedConstants == designExtractor->getConstants());
    }

    SECTION("Multiple procedures constants extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode3);
        std::unordered_set<std::string> expectedConstants = {"1"};
        REQUIRE(expectedConstants == designExtractor->getConstants());
    }

    SECTION("Procedure extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode);
        std::unordered_set<std::string> expectedProcedures = {"main"};
        REQUIRE(expectedProcedures == designExtractor->getProcedures());
    }

    SECTION("Multiple procedure extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode3);
        std::unordered_set<std::string> expectedProcedures = {"main", "test"};
        REQUIRE(expectedProcedures == designExtractor->getProcedures());
    }

    SECTION("Statements extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode);
        std::unordered_set<Stmt> expectedStatements = {
            Stmt{StatementType::ASSIGN, 1}, Stmt{StatementType::READ, 2},   Stmt{StatementType::PRINT, 3},
            Stmt{StatementType::WHILE, 4},  Stmt{StatementType::ASSIGN, 5}, Stmt{StatementType::IF, 6},
            Stmt{StatementType::ASSIGN, 7}, Stmt{StatementType::ASSIGN, 8}, Stmt{StatementType::ASSIGN, 9}};
        REQUIRE(expectedStatements == designExtractor->getStatements());
    }

    SECTION("Single line procedure has statements extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode2);
        std::unordered_set<Stmt> expectedStatements = {Stmt{StatementType::READ, 11}};
        REQUIRE(expectedStatements == designExtractor->getStatements());
    }

    SECTION("Multiple procedure statements extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode3);
        std::unordered_set<Stmt> expectedStatements = {Stmt{StatementType::READ, 11},  Stmt{StatementType::ASSIGN, 1},
                                                       Stmt{StatementType::READ, 2},   Stmt{StatementType::PRINT, 3},
                                                       Stmt{StatementType::WHILE, 4},  Stmt{StatementType::ASSIGN, 5},
                                                       Stmt{StatementType::IF, 6},     Stmt{StatementType::ASSIGN, 7},
                                                       Stmt{StatementType::ASSIGN, 8}, Stmt{StatementType::ASSIGN, 9}};
        REQUIRE(expectedStatements == designExtractor->getStatements());
    }

    SECTION("Follows extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode);
        std::unordered_set<std::pair<StmtNum, StmtNum>> expectedFollows = {{1, 2}, {2, 3}, {3, 4}, {4, 6}, {6, 9}};
        REQUIRE(expectedFollows == designExtractor->getFollows());
    }

    SECTION("Single line procedure has follows extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode2);
        std::unordered_set<std::pair<StmtNum, StmtNum>> expectedFollows = {};
        REQUIRE(expectedFollows == designExtractor->getFollows());
    }

    SECTION("Multiple procedures follows extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode3);
        std::unordered_set<std::pair<StmtNum, StmtNum>> expectedFollows = {{1, 2}, {2, 3}, {3, 4}, {4, 6}, {6, 9}};
        REQUIRE(expectedFollows == designExtractor->getFollows());
    }

    SECTION("Parent extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode);
        std::unordered_set<std::pair<StmtNum, StmtNum>> expectedParent = {{4, 5}, {6, 7}, {6, 8}};
        REQUIRE(expectedParent == designExtractor->getParent());
    }

    SECTION("Single line procedure has parent extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode2);
        std::unordered_set<std::pair<StmtNum, StmtNum>> expectedParent = {};
        REQUIRE(expectedParent == designExtractor->getParent());
    }

    SECTION("Multiple procedures parent extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode3);
        std::unordered_set<std::pair<StmtNum, StmtNum>> expectedParent = {{4, 5}, {6, 7}, {6, 8}};
        REQUIRE(expectedParent == designExtractor->getParent());
    }

    SECTION("Uses statements extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode);
        std::unordered_set<std::pair<StmtNum, Variable>> expectedUses = {
            {1, "y"}, {3, "z"}, {4, "c"}, {4, "d"}, {4, "e"}, {4, "f"}, {4, "h"}, {5, "h"},
            {6, "i"}, {6, "j"}, {6, "k"}, {6, "l"}, {6, "j"}, {7, "j"}, {6, "b"}, {8, "b"}};
        REQUIRE(expectedUses == designExtractor->getUses());
    }

    SECTION("Uses procedures extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode);
        std::unordered_set<std::pair<Procedure, Variable>> expectedProcedureUses = {
            {"main", "y"}, {"main", "z"}, {"main", "c"}, {"main", "d"}, {"main", "e"}, {"main", "f"},
            {"main", "h"}, {"main", "h"}, {"main", "i"}, {"main", "j"}, {"main", "k"}, {"main", "l"},
            {"main", "j"}, {"main", "j"}, {"main", "b"}, {"main", "b"}};
        REQUIRE(expectedProcedureUses == designExtractor->getProcedureUses());
    }

    SECTION("Single line procedure has uses extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode2);
        std::unordered_set<std::pair<StmtNum, Variable>> expectedUses = {};
        REQUIRE(expectedUses == designExtractor->getUses());
    }

    SECTION("Multiple procedures uses statements extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode3);
        std::unordered_set<std::pair<StmtNum, Variable>> expectedUses = {
            {1, "y"}, {3, "z"}, {4, "c"}, {4, "d"}, {4, "e"}, {4, "f"}, {4, "h"}, {5, "h"},
            {6, "i"}, {6, "j"}, {6, "k"}, {6, "l"}, {6, "j"}, {7, "j"}, {6, "b"}, {8, "b"}};
        REQUIRE(expectedUses == designExtractor->getUses());
    }

    SECTION("Multiple procedures uses procedures extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode3);
        std::unordered_set<std::pair<Procedure, Variable>> expectedProcedureUses = {
            {"main", "y"}, {"main", "z"}, {"main", "c"}, {"main", "d"}, {"main", "e"}, {"main", "f"},
            {"main", "h"}, {"main", "h"}, {"main", "i"}, {"main", "j"}, {"main", "k"}, {"main", "l"},
            {"main", "j"}, {"main", "j"}, {"main", "b"}, {"main", "b"}};
        REQUIRE(expectedProcedureUses == designExtractor->getProcedureUses());
    }

    SECTION("Modifies statements extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode);
        std::unordered_set<std::pair<StmtNum, Variable>> expectedModifies = {
            {1, "x"}, {2, "a"}, {4, "g"}, {5, "g"}, {6, "i"}, {6, "a"}, {7, "i"}, {8, "a"}, {9, "x"}};
        REQUIRE(expectedModifies == designExtractor->getModifies());
    }

    SECTION("Modifies procedures extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode);
        std::unordered_set<std::pair<Procedure, Variable>> expectedProcedureModifies = {
            {"main", "x"}, {"main", "a"}, {"main", "g"}, {"main", "g"}, {"main", "i"},
            {"main", "a"}, {"main", "i"}, {"main", "a"}, {"main", "x"}};
        REQUIRE(expectedProcedureModifies == designExtractor->getProcedureModifies());
    }

    SECTION("Single line procedure has modifies extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode2);
        std::unordered_set<std::pair<StmtNum, Variable>> expectedModifies = {{11, "a"}};
        REQUIRE(expectedModifies == designExtractor->getModifies());
    }

    SECTION("Multiple procedures modifies statements extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode3);
        std::unordered_set<std::pair<StmtNum, Variable>> expectedModifies = {
            {11, "a"}, {2, "a"}, {4, "g"}, {5, "g"}, {6, "i"}, {6, "a"}, {7, "i"}, {8, "a"}, {9, "x"}, {1, "x"}};
        REQUIRE(expectedModifies == designExtractor->getModifies());
    }

    SECTION("Multiple procedures modifies procedures extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode3);
        std::unordered_set<std::pair<Procedure, Variable>> expectedProcedureModifies = {
            {"test", "a"}, {"main", "a"}, {"main", "g"}, {"main", "g"}, {"main", "i"}, {"main", "a"},
            {"main", "i"}, {"main", "a"}, {"main", "x"}, {"main", "x"}, {"main", "a"}};
        REQUIRE(expectedProcedureModifies == designExtractor->getProcedureModifies());
    }

    SECTION("Pattern extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode);
        std::unordered_set<std::pair<StmtNum, std::pair<std::string, std::string>>> expectedPattern = {
            {1, {"x", "y"}}, {5, {"g", "h"}}, {7, {"i", "j"}}, {8, {"a", "b"}}, {9, {"x", "1"}}};
        REQUIRE(expectedPattern == designExtractor->getPattern());
    }

    SECTION("Single line procedure has pattern extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode2);
        std::unordered_set<std::pair<StmtNum, std::pair<std::string, std::string>>> expectedPattern = {};
        REQUIRE(expectedPattern == designExtractor->getPattern());
    }

    SECTION("Multiple procedures pattern extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNode3);
        std::unordered_set<std::pair<StmtNum, std::pair<std::string, std::string>>> expectedPattern = {
            {1, {"x", "y"}}, {5, {"g", "h"}}, {7, {"i", "j"}}, {8, {"a", "b"}}, {9, {"x", "1"}}};
        REQUIRE(expectedPattern == designExtractor->getPattern());
    }

    /* THIS TEST IS FOR CALLS ONLY
       procedure sad {
        1 call happy;
       }
       procedure happy {
        2 print a;
        3 read b;
       }
        */

    auto callNode = std::make_shared<CallNode>("happy", 1);
    std::vector<std::shared_ptr<StatementNode>> childrenCall = {};
    childrenCall.push_back(callNode);
    auto stmtListProcCall = std::make_shared<StatementListNode>(childrenCall);
    auto ProcNodeCall = std::make_shared<ProcedureNode>("sad", stmtListProcCall);

    auto variableCall = std::make_shared<VariableNode>("a", 2);
    auto printCall = std::make_shared<PrintNode>(variableCall, 2);

    auto variableCall2 = std::make_shared<VariableNode>("b", 3);
    auto readCall = std::make_shared<ReadNode>(variableCall2, 3);

    std::vector<std::shared_ptr<StatementNode>> childrenCall2 = {};
    childrenCall2.push_back(printCall);
    childrenCall2.push_back(readCall);
    auto stmtListProcCall2 = std::make_shared<StatementListNode>(childrenCall2);
    auto ProcNodeCall2 = std::make_shared<ProcedureNode>("happy", stmtListProcCall2);

    std::vector<std::shared_ptr<ProcedureNode>> childrenProgCall = {};

    childrenProgCall.push_back(ProcNodeCall);
    childrenProgCall.push_back(ProcNodeCall2);
    auto ProgNodeCall = std::make_shared<ProgramNode>(childrenProgCall);

    SECTION("Calls extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNodeCall);
        std::unordered_set<std::pair<Procedure, Procedure>> expectedCalls = {{"sad", "happy"}};
        REQUIRE(expectedCalls == designExtractor->getCalls());
    }

    SECTION("Uses extracted correctly for call statements") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNodeCall);
        std::unordered_set<std::pair<StmtNum, Variable>> expectedUses = {{1, "a"}, {2, "a"}};
        REQUIRE(expectedUses == designExtractor->getUses());
    }

    SECTION("Modifies extracted correctly for call statements") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNodeCall);
        std::unordered_set<std::pair<StmtNum, Variable>> expectedModifies = {{1, "b"}, {3, "b"}};
        REQUIRE(expectedModifies == designExtractor->getModifies());
    }

    SECTION("Uses for procedures with call statements extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNodeCall);
        std::unordered_set<std::pair<Procedure, Variable>> expectedProcedureUses = {
            {"sad", "a"}, {"sad", "a"}, {"happy", "a"}};
        REQUIRE(expectedProcedureUses == designExtractor->getProcedureUses());
    }

    SECTION("Modifies for procedures with call statements extracted correctly") {
        DesignExtractor *designExtractor = new DesignExtractor();
        designExtractor->extract(ProgNodeCall);
        std::unordered_set<std::pair<Procedure, Variable>> expectedProcedureModifies = {
            {"sad", "b"}, {"happy", "b"}, {"sad", "b"}};
        REQUIRE(expectedProcedureModifies == designExtractor->getProcedureModifies());
    }
}
