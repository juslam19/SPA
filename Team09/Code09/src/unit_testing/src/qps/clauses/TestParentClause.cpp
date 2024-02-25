#include "PKB/PKBClient/PKBFacadeWriter.h"
#include "TesterHelper.h"
#include "catch.hpp"
#include "qps/clauseArguments/Wildcard.h"
#include "qps/clauses/SuchThatClause.h"

TEST_CASE("SuchThatClause evaluate for parent relationship with no synonyms") {
    PKB pkb{};
    PKBFacadeWriter pfw{pkb};
    PKBFacadeReader pfr{pkb};

    SECTION("Parent(Integer, Integer)") {
        std::unordered_set<std::pair<int, int>> parentStoreEntries{std::pair<int, int>{1, 2}};
        pfw.setParentStore(parentStoreEntries);
        ParentTester{pfr, new Integer("1"), new Integer("2")}.testBoolean(true);
        ParentTester{pfr, new Integer("2"), new Integer("1")}.testBoolean(false);
        ParentTester{pfr, new Integer("1"), new Integer("1")}.testBoolean(false);
        ParentTester{pfr, new Integer("2"), new Integer("2")}.testBoolean(false);
    }

    SECTION("Parent(Integer, Integer) / empty store") {
        pfw.setParentStore({});
        ParentTester{pfr, new Integer("1"), new Integer("2")}.testBoolean(false);
    }

    SECTION("Parent(Integer, Wildcard)") {
        std::unordered_set<std::pair<int, int>> parentStoreEntries{std::pair<int, int>{1, 2}};
        pfw.setParentStore(parentStoreEntries);
        ParentTester{pfr, new Integer("1"), new Wildcard()}.testBoolean(true);
        ParentTester{pfr, new Integer("2"), new Wildcard()}.testBoolean(false);
    }

    SECTION("Parent(Integer, Wildcard) / empty store") {
        pfw.setParentStore({});
        ParentTester{pfr, new Integer("1"), new Wildcard()}.testBoolean(false);
        ParentTester{pfr, new Wildcard(), new Wildcard()}.testBoolean(false);
    }

    SECTION("Parent(Wildcard, Integer)") {
        std::unordered_set<std::pair<int, int>> parentStoreEntries{std::pair<int, int>{1, 2}};
        pfw.setParentStore(parentStoreEntries);
        ParentTester{pfr, new Wildcard(), new Integer("2")}.testBoolean(true);
        ParentTester{pfr, new Wildcard(), new Integer("1")}.testBoolean(false);
    }

    SECTION("Parent(Wildcard, Wildcard)") {
        std::unordered_set<std::pair<int, int>> parentStoreEntries{std::pair<int, int>{1, 2}};
        pfw.setParentStore(parentStoreEntries);
        ParentTester{pfr, new Wildcard(), new Wildcard()}.testBoolean(true);
    }

    SECTION("Parent(Wildcard, Wildcard) / empty store") {
        pfw.setParentStore({});
        ParentTester{pfr, new Wildcard(), new Wildcard()}.testBoolean(false);
    }
}

TEST_CASE("SuchThatClause evaluate for parent relationship with 1 synonym") {
    PKB pkb{};
    PKBFacadeWriter pfw{pkb};
    PKBFacadeReader pfr{pkb};

    SECTION("Parent(Synonym, Integer)") {
        std::unordered_set<std::pair<int, int>> parentStoreEntries{std::pair<int, int>{1, 2},
                                                                   std::pair<int, int>{1, 3}};
        pfw.setParentStore(parentStoreEntries);
        Synonym* stmtSyn = new Synonym(DesignEntityType::STMT, "s");

        // Select s such that Parent(s, 2)
        ParentTester{pfr, stmtSyn, new Integer("2")}.testSynonyms({*stmtSyn}).testSynonymValues({{"1"}});
        // Select s such that Parent(s, 3)
        ParentTester{pfr, stmtSyn, new Integer("3")}.testSynonyms({*stmtSyn}).testSynonymValues({{"1"}});
        // Select s such that Parent(s, 1)
        ParentTester{pfr, stmtSyn, new Integer("1")}.testSynonyms({*stmtSyn}).testSynonymValues({{}});

        // Select s such that Parent(s, 5), out of bounds
        ParentTester{pfr, stmtSyn, new Integer("5")}.testSynonyms({*stmtSyn}).testSynonymValues({{}});
    }

    SECTION("Parent(Integer, Synonym)") {
        std::unordered_set<std::pair<int, int>> parentStoreEntries{std::pair<int, int>{1, 2},
                                                                   std::pair<int, int>{1, 3}};
        pfw.setParentStore(parentStoreEntries);
        Synonym* stmtSyn = new Synonym(DesignEntityType::STMT, "s");

        // Select s such that Parent(1, s)
        ParentTester{pfr, new Integer("1"), stmtSyn}.testSynonyms({*stmtSyn}).testSynonymValues({{"2", "3"}});
        // Select s such that Parent(2, s)
        ParentTester{pfr, new Integer("2"), stmtSyn}.testSynonyms({*stmtSyn}).testSynonymValues({{}});
        // Select s such that Parent(3, s)
        ParentTester{pfr, new Integer("3"), stmtSyn}.testSynonyms({*stmtSyn}).testSynonymValues({{}});

        // Select s such that Parent(5, s), out of bounds
        ParentTester{pfr, new Integer("5"), stmtSyn}.testSynonyms({*stmtSyn}).testSynonymValues({{}});
    }

    SECTION("Parent(Synonym, Wildcard)") {
        std::unordered_set<Stmt> stmts = {Stmt{StatementType::ASSIGN, 1}, Stmt{StatementType::ASSIGN, 2},
                                          Stmt{StatementType::ASSIGN, 3}};

        std::unordered_set<std::pair<int, int>> parentStoreEntries{std::pair<int, int>{1, 2},
                                                                   std::pair<int, int>{1, 3}};

        pfw.setStmts(stmts);
        pfw.setParentStore(parentStoreEntries);
        Synonym* stmtSyn = new Synonym(DesignEntityType::STMT, "s");

        // Select s such that Parent(s, _)
        ParentTester{pfr, stmtSyn, new Wildcard()}.testSynonyms({*stmtSyn}).testSynonymValues({{"1"}});
    }

    SECTION("Parent(Wildcard, Synonym)") {
        std::unordered_set<Stmt> stmts = {Stmt{StatementType::ASSIGN, 1}, Stmt{StatementType::ASSIGN, 2},
                                          Stmt{StatementType::ASSIGN, 3}};

        std::unordered_set<std::pair<int, int>> parentStoreEntries{std::pair<int, int>{1, 2},
                                                                   std::pair<int, int>{1, 3}};

        pfw.setStmts(stmts);
        pfw.setParentStore(parentStoreEntries);
        Synonym* stmtSyn = new Synonym(DesignEntityType::STMT, "s");

        // Select s such that Parent(_, s)
        ParentTester{pfr, new Wildcard(), stmtSyn}.testSynonyms({*stmtSyn}).testSynonymValues({{"2", "3"}});
    }
}

TEST_CASE("SuchThatClause evaluate for parent relationship with 2 synonyms") {
    PKB pkb{};
    PKBFacadeWriter pfw{pkb};
    PKBFacadeReader pfr{pkb};

    SECTION("Parent(Synonym, Synonym)") {
        std::unordered_set<std::pair<int, int>> parentStoreEntries{std::pair<int, int>{1, 2}, std::pair<int, int>{1, 3},
                                                                   std::pair<int, int>{3, 4}};

        std::unordered_set<Stmt> stmts = {Stmt{StatementType::ASSIGN, 1}, Stmt{StatementType::ASSIGN, 2},
                                          Stmt{StatementType::ASSIGN, 3}, Stmt{StatementType::ASSIGN, 4}};

        pfw.setStmts(stmts);
        pfw.setParentStore(parentStoreEntries);
        Synonym* stmtSyn1 = new Synonym(DesignEntityType::STMT, "s1");
        Synonym* stmtSyn2 = new Synonym(DesignEntityType::STMT, "s2");

        // Select s1 such that Parent(s1, s2)
        ParentTester{pfr, stmtSyn1, stmtSyn2}
            .testSynonyms({*stmtSyn1, *stmtSyn2})
            .testSynonymValues({{"1", "1", "3"}, {"2", "3", "4"}});
    }
}
