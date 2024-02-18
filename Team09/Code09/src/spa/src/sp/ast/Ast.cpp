#include <sp/ast/Ast.h>

#include <deque>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "sp/exceptions/SyntaxError.h"

/*
root node is fixed so we create that at the top first before traversing the
token vector. now we process the procedures each procedure.  procedure
<identifier> { stmtLIst+ } So the first token should be the procedure keyword


Assumptions:
- First token is always procedure
- Program is syntatically correct
*/

ProgramNode AST::buildAST(std::vector<Token> tokens) {
    ProgramNode root = ProgramNode();
    std::deque<Token> token_deque(tokens.begin(), tokens.end());

    while (token_deque.size()) {
        ProcedureNode procedure = buildProcedureAST(token_deque);
        root.add_child(procedure);
    }

    return root;
}
/*
Grammar: 'procedure' proc_name { stmtList }
The first word should be procedure, so we check for that by looking at the first
element of the deque. Otherwise, we throw a syntax error. Then we look at the
next token to see if its a name or not then finally we process statement lists,
which is enclosed in {}
*/
ProcedureNode AST::buildProcedureAST(std::deque<Token>& tokens) {
    Token procedureKeyword = tokens.front();
    checkSyntax(PROCEDURE, procedureKeyword.type);
    tokens.pop_front();
    Token procName = tokens.front();
    checkSyntax(NAME, procName.type);
    // if everything is valid, we can create the node
    ProcedureNode node = ProcedureNode(procName.value);
    tokens.pop_front();

    StatementListNode statementList = buildStatementListAST(tokens);
    node.add_child(statementList);
    return node;
}

/*
stmt list passes here includes the curly braces, so we can check for that as
well.

*/
StatementListNode AST::buildStatementListAST(std::deque<Token>& tokens) {
    Token openCurly = tokens.front();
    checkSyntax(OPEN_CURLY_BRACE, openCurly.type);
    tokens.pop_front();

    StatementListNode statementList = StatementListNode();
    std::vector<StatementNode> statements = {};
    while (tokens.size() && tokens.front().type != CLOSE_CURLY_BRACE) {
        StatementNode statement = buildStatementAST(tokens);
        statementList.add_child(statement);
    }

    checkMissingToken(CLOSE_CURLY_BRACE, tokens);
    Token closeCurly = tokens.front();
    checkSyntax(CLOSE_CURLY_BRACE, closeCurly.type);
    tokens.pop_front();
    return statementList;
}

StatementNode AST::buildStatementAST(std::deque<Token>& tokens) {
    Token first_token = tokens.front();
    StatementNode statement = StatementNode();
    if (first_token.type == NAME) {
        AssignmentNode assignment = buildAssignmentAST(tokens);
        statement.add_child(assignment);
        return statement;
    }
    throw UnrecognisedTokenError(first_token.type);
}

AssignmentNode AST::buildAssignmentAST(std::deque<Token>& tokens) {
    Token varName = tokens.front();
    NameNode nameNode = NameNode(varName.value);
    tokens.pop_front();
    Token equality = tokens.front();
    checkSyntax(EQUAL, equality.type);
    AssignmentNode node = AssignmentNode();
    tokens.pop_front();
    ExpressionNode expression = buildExpressionAST(tokens);
    Token semiColon = tokens.front();
    checkSyntax(SEMICOLON, semiColon.type);
    tokens.pop_front();

    node.add_child(nameNode);
    node.add_child(expression);
    return node;
}

/*
To perform expression parsing, we require manipulating left recursion. To
maintain the integrity of our queue which gives us elements from left to right.
The idea is to remove left recursion somehow...

Rule: A: A '+' B | A '-' B | B.
We can express this rule in terms of x, y where x is a non-empty sequence of
nonterminal and terminal, and y is a sequence of nonterminals and terminals that
does not start with A.

Rewriting the rule, we get the following rule:
A: A(x1) | A(x2) | y, x1 represents '+' B and x2 represents '-' B.

We can split the rule above into 2 sets, one for A, and another for
nonterminals, A'. A: BA' A': (x1)A' | (x2)A' | e. e represents an empty string.
So the rules we will be solving for becomes the following:
A: BA'
A': '+' BA' | '-' BA' | e

More info can be found here: https://en.wikipedia.org/wiki/Left_recursion

*/
ExpressionNode AST::buildExpressionAST(std::deque<Token>& tokens) {
    TermNode term = buildTermAST(tokens);
    return buildSubExpressionAST(tokens, &term);
}

ExpressionNode AST::buildSubExpressionAST(std::deque<Token>& tokens,
                                          ExpressionNode* node) {
    Token front = tokens.front();
    if (front.type == ADD || front.type == SUB) {
        TermNode term = buildTermAST(tokens);
        tokens.pop_front();
        ExpressionNode expressionNode = ExpressionNode(front.type);
        expressionNode.add_child(*node);
        expressionNode.add_child(term);

        return buildSubExpressionAST(tokens, &expressionNode);
    }

    return *node;
}

/*
Similar idea to buildExpression, because of its left recursive nature, we can
perform the same thing. The rules will be transformed from: A : A '*' B | A '/'
B | A '%' B | B

to the following 2 rules:
A : BA'
A' : '*' BA' | '/' BA' | '%' BA' | e
*/
TermNode AST::buildTermAST(std::deque<Token>& tokens) {
    FactorNode factorNode = buildFactorAST(tokens);
    return buildSubTermAST(tokens, &factorNode);
}

TermNode AST::buildSubTermAST(std::deque<Token>& tokens, TermNode* node) {
    Token front = tokens.front();
    if (front.type == MUL || front.type == MOD || front.type == DIV) {
        FactorNode factorNode = buildFactorAST(tokens);
        tokens.pop_front();
        TermNode termNode = TermNode(front.type);
        termNode.add_child(*node);
        termNode.add_child(factorNode);

        return buildSubTermAST(tokens, &termNode);
    }

    return *node;
}

// For now just assume theres no bracketed expressions
FactorNode AST::buildFactorAST(std::deque<Token>& tokens) {
    Token token = tokens.front();
    tokens.pop_front();
    if (token.type == NAME) {
        return NameNode(token.value);
    }
    return IntegerNode(token.value);
}

/*
------------------------------------------

Syntax Error methods

------------------------------------------
*/

void AST::checkMissingToken(LEXICAL_TOKEN_TYPE expected,
                            std::deque<Token>& tokens) {
    if (tokens.size() != 0) {
        return;
    }
    throw MissingTokenError(expected);
}

void AST::checkSyntax(LEXICAL_TOKEN_TYPE expected,
                      LEXICAL_TOKEN_TYPE received) {
    if (expected == received) {
        return;
    }
    throw LexicalSyntaxError(expected, received);
}
