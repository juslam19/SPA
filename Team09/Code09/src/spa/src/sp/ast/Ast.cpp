#include <sp/ast/Ast.h>

#include <deque>
#include <iostream>
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
    std::deque<Token> token_deque;
    for (auto token : tokens) {
        token_deque.push_back(token);
    }

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
ProcedureNode AST::buildProcedureAST(std::deque<Token> tokens) {
    Token procedureKeyword = tokens.front();
    checkSyntax(PROCEDURE, procedureKeyword.type);
    tokens.pop_front();
    Token procName = tokens.front();
    checkSyntax(NAME, procName.type);
    // if everything is valid, we can create the node
    ProcedureNode node = ProcedureNode(procName.value);

    StatementListNode statementList = buildStatementListAST(tokens);
    node.add_child(statementList);
    return node;
}

/*
stmt list passes here includes the curly braces, so we can check for that as
well.

*/
StatementListNode AST::buildStatementListAST(std::deque<Token> tokens) {
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

/*
There are 5 kinds of statements, with assignment having no keyword.
So we check whether the first token is a var_name. This will tell us that it is
an assignment. Otherwise, we just check the other nodes accordingly.
*/
StatementNode AST::buildStatementAST(std::deque<Token> tokens) {
    Token first_token = tokens.front();
    StatementNode statement = StatementNode();
    if (first_token.type == NAME) {
        AssignmentNode assignment = buildAssignmentAST(tokens);
        statement.add_child(assignment);
        return statement;
    }
    throw UnrecognisedTokenError(first_token.type);
}

AssignmentNode AST::buildAssignmentAST(std::deque<Token> tokens) {
    Token varName = tokens.front();
    NameNode nameNode = NameNode(varName.value);
    tokens.pop_front();
    Token equality = tokens.front();
    checkSyntax(EQUAL, equality.type);
    AssignmentNode node = AssignmentNode();
    ExpressionNode expression = buildExpressionAST(tokens);
    Token semiColon = tokens.front();
    checkSyntax(SEMICOLON, semiColon.type);

    node.add_child(nameNode);
    node.add_child(expression);
    return node;
}

/*
We pop things from the right of the deque until we hit either a  '+' or a '-'.
Once we have hit a symbol, the remaining elements of the deque will be passed
back to expr. The initial elements would be stored in another deque that will be
passed to term for processing.
*/
ExpressionNode AST::buildExpressionAST(std::deque<Token> tokens) {
    std::deque<Token> term_deque = {};
    LEXICAL_TOKEN_TYPE type;
    while (tokens.size()) {
        Token lastToken = tokens.back();
        if (lastToken.type == ADD || lastToken.type == SUB) {
            type = lastToken.type;
            break;
        }
        tokens.pop_back();
        term_deque.push_back(lastToken);
    }
    // if there are no more tokens, that means that we found a term instead
    if (tokens.size() == 0) {
        return buildTermAST(term_deque);
    }

    return buildBinaryExpressionAST(tokens, term_deque, type);
}

ExpressionNode AST::buildBinaryExpressionAST(std::deque<Token> tokens,
                                             std::deque<Token> term,
                                             LEXICAL_TOKEN_TYPE type) {
    ExpressionNode binaryExpressionNode = ExpressionNode(type);
    ExpressionNode expressionNode = buildExpressionAST(tokens);
    TermNode termNode = buildTermAST(term);
    binaryExpressionNode.add_child(expressionNode);
    binaryExpressionNode.add_child(termNode);
    return binaryExpressionNode;
}

TermNode AST::buildTermAST(std::deque<Token> tokens) {
    LEXICAL_TOKEN_TYPE type;
    std::deque<Token> factorDeque = {};
    while (tokens.size()) {
        Token lastToken = tokens.back();
        if (lastToken.type == MUL || lastToken.type == DIV ||
            lastToken.type == MOD) {
            type = lastToken.type;
            break;
        }
        factorDeque.push_back(lastToken);
    }

    if (tokens.size() == 0) {
        return buildFactorAST(factorDeque);
    }

    return buildBinaryTermAST(tokens, factorDeque, type);
}

TermNode AST::buildBinaryTermAST(std::deque<Token> tokens,
                                 std::deque<Token> factor,
                                 LEXICAL_TOKEN_TYPE type) {
    TermNode binaryTermNode = TermNode(type);
    TermNode termNode = buildTermAST(tokens);
    FactorNode factorNode = buildFactorAST(factor);
    binaryTermNode.add_child(termNode);
    binaryTermNode.add_child(factorNode);
    return binaryTermNode;
}

// For now just assume theres no bracketed expressions
FactorNode AST::buildFactorAST(std::deque<Token> tokens) {
    Token token = tokens.front();
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
                            std::deque<Token> tokens) {
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

// /*
// Finds a procedure by comparing closing curly braces.
// The algorithm uses a stack to do so. Open curly braces are always pushed onto
// the stack. If there are closes curly braces, we pop_front from the stack if
// the last element is an open curly brace. Once the stack is empty, we are done
// with the currnt procedure. So we add it into our resulting array.
// */
// std::vector<std::vector<Token>> AST::splitByProcedure(
//     std::vector<Token> tokens) {
//   std::vector<std::vector<Token>> procedures = {};
//   std::vector<Token> current_procedure = {};
//   std::stack<Token> stack = {};
//   for (auto token : tokens) {
//     if (token.type == LEXICAL_TOKEN_TYPE::OPEN_CURLY_BRACE) {
//       stack.push(token);

//     } else if (token.type == LEXICAL_TOKEN_TYPE::CLOSE_CURLY_BRACE &&
//                stack.top().type == LEXICAL_TOKEN_TYPE::OPEN_CURLY_BRACE) {
//       stack.pop_front();
//     }
//     current_procedure.push_back(token);
//     // if the current token we pushed is a proc type, or the prev token
//     // before this one is, we dont pop_front from the stack
//     // TODO(ben): handle nested procedures
//     if (token.type == PROCEDURE ||
//         current_procedure.end()[-2].type == PROCEDURE) {
//       continue;
//     }
//     // if the stack is empty, means that we have reached the end of the
//     // current procedure. so we add the current procedure to the procedures
//     // vect and start again.
//     if (stack.empty()) {
//       procedures.push_back(current_procedure);
//       current_procedure = {};
//     }
//   }

//   return procedures;
// }

// /*
// Takes in some tokens from a given procedure. Get all the statements by
// comparing on semicolon (this is only correct for some statements)
// TODO(ben): split by other types of statements
// */
// std::vector<std::vector<Token>> AST::splitByStatements(
//     std::vector<Token> tokens) {
//   std::vector<std::vector<Token>> statements = {};
//   std::vector<Token> current_statement = {};
//   for (auto token : tokens) {
//     if (token.type == LEXICAL_TOKEN_TYPE::SEMICOLON) {
//       statements.push_back(current_statement);
//       current_statement = {};
//     } else {
//       current_statement.push_back(token);
//     }
//   }

//   return statements;
// }

// /*
// Since procedures are made of statements, we need to find all the statements
// first. Then we build the statement ast for each statement and add it to the
// statement list. Currently we assume that every statement is an assignment.
// */
// ASTNode AST::buildProcedureAST(std::deque<Token> tokens) {

//   ASTNode procedure = ASTNode(tokens[1].value, "proc");
//   ASTNode stmtList = ASTNode("", "stmtList");
//   // need to remove the first 3 tokens (procedure XXXX {), and the last
//   // TODO(ben): find a better way to do this  std::vector<Token>
//   std::vector<Token> statementListTokens =
//       std::vector<Token>(tokens.begin() + 3, tokens.end() - 1);
//   std::vector<std::vector<Token>> statements =
//       splitByStatements(statementListTokens);
//   for (auto statement : statements) {
//     ASTNode statementAST = buildAssignmentAST(statement);
//     stmtList.add_child(statementAST);
//   }
//   procedure.add_child(stmtList);
//   return procedure;
// }

// /*
// Once we reach the equal sign, we build the LHS AST. Then we continue using
// the same vector for the RHS. At the end, we build RHS and then we add them as
// children. Grammar for assignment is: var_name '=' expr ';'

// So before the equal, we build the var name ast
// */
// ASTNode AST::buildAssignmentAST(std::vector<Token> tokens) {
//   ASTNode root = ASTNode("", "assign");
//   std::vector<Token> token_vector = {};
//   for (auto token : tokens) {
//     if (token.type == LEXICAL_TOKEN_TYPE::EQUAL) {
//       ASTNode identifierAST = ASTNode(tokens[0].value, "var");
//       root.add_child(identifierAST);
//       token_vector = {};
//     } else {
//       token_vector.push_back(token);
//     }
//   }
//   ASTNode expressionAST = buildExpressionAST(token_vector);
//   root.add_child(expressionAST);
//   return root;
// }

// /*
// attempt to find a '+' or '-' otherwise, we know that its just a term. If its
// just a term, we build the term ast instead.
// */
// ASTNode AST::buildExpressionAST(std::vector<Token> tokens) {
//   for (int i = tokens.size() - 1; i > 0; i--) {
//     Token current_token = tokens[i];
//     if (current_token.type == ADD) {
//       return buildBinaryExpressionAST(tokens, "add", i);
//     } else if (current_token.type == SUB) {
//       return buildBinaryExpressionAST(tokens, "sub", i);
//     }
//   }

//   return buildTermAST(tokens);
// }

// ASTNode AST::buildBinaryExpressionAST(std::vector<Token> tokens,
//                                       std::string ast_node_type,
//                                       int operator_index) {
//   ASTNode root = ASTNode("", ast_node_type);
//   std::vector<Token> expression_tokens =
//       std::vector<Token>(tokens.begin(), tokens.begin() + operator_index);
//   std::vector<Token> term_tokens =
//       std::vector<Token>(tokens.begin() + operator_index + 1, tokens.end());

//   ASTNode expression = buildExpressionAST(expression_tokens);
//   ASTNode term = buildTermAST(term_tokens);
//   root.add_child(expression);
//   root.add_child(term);
//   return root;
// }

// /*
// Similar idea to build expression, except now we split by different operators
// instead
// */
// ASTNode AST::buildTermAST(std::vector<Token> tokens) {
//   for (int i = tokens.size() - 1; i > 0; i--) {
//     Token current_token = tokens[i];
//     if (current_token.type == MUL) {
//       return buildBinaryTermAST(tokens, "mul", i);
//     } else if (current_token.type == DIV) {
//       return buildBinaryTermAST(tokens, "div", i);
//     } else if (current_token.type == MOD) {
//       return buildBinaryTermAST(tokens, "mod", i);
//     }
//   }

//   return buildFactorAST(tokens);
// }

// ASTNode AST::buildBinaryTermAST(std::vector<Token> tokens,
//                                 std::string ast_node_type, int
//                                 operator_index) {
//   ASTNode root = ASTNode("", ast_node_type);
//   std::vector<Token> term_tokens =
//       std::vector<Token>(tokens.begin(), tokens.begin() + operator_index);
//   std::vector<Token> factor_tokens =
//       std::vector<Token>(tokens.begin() + operator_index + 1, tokens.end());

//   ASTNode term = buildTermAST(term_tokens);
//   ASTNode factor = buildFactorAST(factor_tokens);
//   root.add_child(term);
//   root.add_child(factor);
//   return root;
// }

// /*
// For now we assume that factors are only identifiers.
// */
// ASTNode AST::buildFactorAST(std::vector<Token> tokens) {
//   Token identifier = tokens[0];
//   if (identifier.type == NAME) {
//     return ASTNode(tokens[0].value, "var");
//   }

//   return ASTNode(identifier.value, "const");
// }
