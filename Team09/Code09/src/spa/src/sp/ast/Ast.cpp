#include <sp/ast/Ast.h>

#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <unordered_set>

#include "sp/de/AstVisitor.h"

/*
root node is fixed so we create that at the top first before traversing the
token vector. now we process the procedures each procedure.  procedure
<identifier> { stmtLIst+ } So the first token should be the procedure keyword


Assumptions:
- First token is always procedure
- Program is syntatically correct
*/

std::shared_ptr<ProgramNode> AST::buildAST(std::vector<Token> tokens) {
    std::queue<Token> token_queue = {};
    for (auto token : tokens) {
        token_queue.push(token);
    }

    std::vector<std::shared_ptr<ProcedureNode>> children = {};

    while (token_queue.size()) {
        std::shared_ptr<ProcedureNode> procedure = buildProcedureAST(token_queue);
        children.push_back((procedure));
    }

    std::shared_ptr<ProgramNode> programNode = std::make_shared<ProgramNode>(children);

    semanticValidator.validateSemantics(programNode);

    return programNode;
}
/*
Grammar: 'procedure' proc_name { stmtList }
The first word should be procedure, so we check for that by looking at the first
element of the queue. Otherwise, we throw a syntax error. Then we look at the
next token to see if its a name or not then finally we process statement lists,
which is enclosed in {}
*/
std::shared_ptr<ProcedureNode> AST::buildProcedureAST(std::queue<Token>& tokens) {
    tokens.pop();  // remove 'procedure' keyword
    Token procName = tokens.front();
    tokens.pop();  // remove proc_name

    std::shared_ptr<StatementListNode> statementList = buildStatementListAST(tokens);
    return std::make_shared<ProcedureNode>(procName.value, statementList);
}

/*
stmt list passes here includes the curly braces, so we can check for that as
well.

*/
std::shared_ptr<StatementListNode> AST::buildStatementListAST(std::queue<Token>& tokens) {
    tokens.pop();  // remove {
    std::vector<std::shared_ptr<StatementNode>> children = {};

    while (tokens.size() && tokens.front().type != CLOSE_CURLY_BRACE) {
        children.push_back(buildStatementAST(tokens));
    }

    tokens.pop();  // remove }
    return std::make_shared<StatementListNode>(children);
}

std::shared_ptr<StatementNode> AST::buildStatementAST(std::queue<Token>& tokens) {
    Token first_token = tokens.front();
    if (first_token.type == NAME) {
        return buildAssignmentAST(tokens);
    } else if (first_token.type == READ) {
        return buildReadAST(tokens);
    } else if (first_token.type == PRINT) {
        return buildPrintAST(tokens);
    } else if (first_token.type == WHILE) {
        return buildWhileAST(tokens);
    } else if (first_token.type == CALL) {
        return buildCallAST(tokens);
    }
    return buildIfAST(tokens);
}

std::shared_ptr<IfNode> AST::buildIfAST(std::queue<Token>& tokens) {
    Token ifToken = tokens.front();

    tokens.pop();  // remove if
    std::shared_ptr<ExpressionNode> conditionalExpression = handleBracketedCondExpr(tokens);
    tokens.pop();  // remove then
    std::shared_ptr<StatementListNode> thenStatementList = buildStatementListAST(tokens);
    tokens.pop();  // remove else
    std::shared_ptr<StatementListNode> elseStatementList = buildStatementListAST(tokens);

    return std::make_shared<IfNode>(conditionalExpression, thenStatementList, elseStatementList, ifToken.line_number);
}

std::shared_ptr<WhileNode> AST::buildWhileAST(std::queue<Token>& tokens) {
    // remove the keyword
    Token whileToken = tokens.front();
    tokens.pop();
    std::shared_ptr<ExpressionNode> whileCondition = handleBracketedCondExpr(tokens);
    std::shared_ptr<StatementListNode> statements = buildStatementListAST(tokens);
    return std::make_shared<WhileNode>(whileCondition, statements, whileToken.line_number);
}

std::shared_ptr<ReadNode> AST::buildReadAST(std::queue<Token>& tokens) {
    Token readToken = tokens.front();
    tokens.pop();  // remove read
    Token varName = tokens.front();
    std::shared_ptr<VariableNode> nameNode = buildVarNameAST(varName);
    tokens.pop();  // remove the variable
    tokens.pop();  // remove the semicolon
    return std::make_shared<ReadNode>(nameNode, readToken.line_number);
}

std::shared_ptr<CallNode> AST::buildCallAST(std::queue<Token>& tokens) {
    Token callToken = tokens.front();
    tokens.pop();  // remove call
    Token procedureToken = tokens.front();
    tokens.pop();  // remove the variable
    tokens.pop();  // remove the semicolon
    return std::make_shared<CallNode>(procedureToken.value, callToken.line_number);
}

std::shared_ptr<PrintNode> AST::buildPrintAST(std::queue<Token>& tokens) {
    Token printToken = tokens.front();
    tokens.pop();  // remove print
    Token varName = tokens.front();
    std::shared_ptr<VariableNode> nameNode = buildVarNameAST(varName);
    tokens.pop();  // remove variable
    tokens.pop();  // remove semicolon
    return std::make_shared<PrintNode>(nameNode, varName.line_number);
}

std::shared_ptr<AssignmentNode> AST::buildAssignmentAST(std::queue<Token>& tokens) {
    Token varName = tokens.front();
    std::shared_ptr<VariableNode> nameNode = buildVarNameAST(varName);
    tokens.pop();  // remove the variable name
    Token assignmentToken = tokens.front();
    tokens.pop();  // remove =
    std::shared_ptr<ExpressionNode> expression = buildExpressionAST(tokens);
    tokens.pop();  // remove the expression part
    return std::make_shared<AssignmentNode>(nameNode, expression, assignmentToken.line_number);
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
std::shared_ptr<ExpressionNode> AST::buildExpressionAST(std::queue<Token>& tokens) {
    /*
    We perform upcasting here because there is a chance of the following flow
    happening: Expr -> term -> factor -> expr.
    */
    std::shared_ptr<ExpressionNode> term = buildTermAST(tokens);
    return buildSubExpressionAST(tokens, term);
}

std::shared_ptr<ExpressionNode> AST::buildSubExpressionAST(std::queue<Token>& tokens,
                                                           std::shared_ptr<ExpressionNode> node) {
    if (tokens.size() == 0) {
        return node;
    }
    Token front = tokens.front();
    if (front.type == ADD || front.type == SUB) {
        tokens.pop();
        std::shared_ptr<ExpressionNode> term = buildTermAST(tokens);
        return buildSubExpressionAST(tokens,
                                     std::make_shared<ExpressionNode>(front.type, node, term, front.line_number));
    }
    return node;
}
/*
building the conditional expression ast is slightly tricky. Consider the following flow:
cond_expr -> rel_expr -> expr -> term -> factor -> '(' expr ')' -> ...
because of this, we need to look for either a '!', '&&' or a '||'. If there isnt any before the
current close bracket, we parse it as a rel_expr instaed. To do this, we can follow this algorithm here:
https://www.geeksforgeeks.org/check-for-balanced-parentheses-in-an-expression/. We will just iterate throught the tokens
to find the brackets. In this case, only ( and ). Tokens passed here are guarenteed to start with an open bracket
*/
std::shared_ptr<ExpressionNode> AST::buildBinaryConditionalExpressionAST(std::queue<Token>& tokens) {
    auto tempTokens = tokens;
    std::stack<Token> stack;
    stack.push(tempTokens.front());
    tempTokens.pop();
    bool isRelExpr = true;
    while (tempTokens.size() != 0 && stack.size() != 0) {
        Token current = tempTokens.front();
        if (current.type == OPEN_BRACKET) {
            stack.push(current);

        } else if (current.type == CLOSE_BRACKET) {
            stack.pop();
        } else if (RelationalOperators.find(current.type) != RelationalOperators.end()) {
            isRelExpr = false;
        }
        tempTokens.pop();
    }
    if (isRelExpr) {
        return buildRelationalExpressionAST(tokens);
    }

    std::shared_ptr<ExpressionNode> leftHandSide = handleBracketedCondExpr(tokens);
    // means its just a !(realFactor)
    if (tokens.size() == 0) {
        return leftHandSide;
    }
    Token logicalOperator = tokens.front();
    tokens.pop();  // pop the logical op
    std::shared_ptr<ExpressionNode> rightHandSide = handleBracketedCondExpr(tokens);
    return std::make_shared<ExpressionNode>(logicalOperator.type, leftHandSide, rightHandSide,
                                            logicalOperator.line_number);
}

std::shared_ptr<ExpressionNode> AST::handleBracketedCondExpr(std::queue<Token>& tokens) {
    tokens.pop();
    std::shared_ptr<ExpressionNode> conditionalExpr = buildConditionalExpressionAST(tokens);
    tokens.pop();
    return conditionalExpr;
}

/*
cond_expr: rel_expr | '!' '(' cond_expr ')' |
           '(' cond_expr ')' '&&' '(' cond_expr ')' |
           '(' cond_expr ')' '||' '(' cond_expr ')'

check for '!' first. otherwise check for open brackets. If there are no open brackets,
its guaranteed to be a rel_expr
*/
std::shared_ptr<ExpressionNode> AST::buildConditionalExpressionAST(std::queue<Token>& tokens) {
    if (tokens.front().type == NOT) {
        Token notNode = tokens.front();
        tokens.pop();
        std::shared_ptr<ExpressionNode> conditionalExpr = handleBracketedCondExpr(tokens);
        return std::make_shared<ExpressionNode>(notNode.type, conditionalExpr, notNode.line_number);
    } else if (tokens.front().type == OPEN_BRACKET) {
        return buildBinaryConditionalExpressionAST(tokens);
    }

    return buildRelationalExpressionAST(tokens);
}

/*
Grammar: rel_factor '>' rel_factor | rel_factor '>=' rel_factor |
          rel_factor '<' rel_factor | rel_factor '<=' rel_factor |
          rel_factor '==' rel_factor | rel_factor '!=' rel_factor
*/
std::shared_ptr<ExpressionNode> AST::buildRelationalExpressionAST(std::queue<Token>& tokens) {
    std::shared_ptr<ExpressionNode> leftHandNode = buildRelationalFactorAST(tokens);
    Token conditionalOp = tokens.front();
    tokens.pop();
    std::shared_ptr<ExpressionNode> rightHandNode = buildRelationalFactorAST(tokens);
    return std::make_shared<ExpressionNode>(conditionalOp.type, leftHandNode, rightHandNode, conditionalOp.line_number);
}

/*
cond_expr: rel_expr  | '!' '(' cond_expr ')'  | '(' cond_expr ')' _cond_expr
_cond_expr: '&&' '(' cond_expr ')' | '||' '(' cond_expr ')'

rel_expr: rel_factor rel_op rel_factor
rel_op: '>' | '>=' | '<' | '<=' | '==' | '!='

rel_factor: var_name | const_value | expr
A rel_factor will always be terminated by a close bracket (because of rel_expr grammar) or a
rel_op (because of rel_expr grammar). If we check the following token in the queue and realise
those are actually the case, we can attempt to build a var/const. Otherwise we build an expression
*/
std::shared_ptr<ExpressionNode> AST::buildRelationalFactorAST(std::queue<Token>& tokens) {
    // create a temporary queue to access the following elements in the tokens queue
    std::queue<Token> temp = tokens;
    temp.pop();
    if (temp.size() == 0) {
        return buildExpressionAST(tokens);
    }
    Token followingToken = temp.front();
    Token token = tokens.front();
    if (followingToken.type == CLOSE_BRACKET ||
        RelationalOperators.find(followingToken.type) != RelationalOperators.end()) {
        tokens.pop();
        if (token.type == NAME) {
            return buildVarNameAST(token);
        }
        return buildIntAST(token);
    }
    return buildExpressionAST(tokens);
}

/*
Similar idea to buildExpression, because of its left recursive nature, we can
perform the same thing. The rules will be transformed from: A : A '*' B | A '/'
B | A '%' B | B

to the following 2 rules:
A : BA'
A' : '*' BA' | '/' BA' | '%' BA' | e
*/
std::shared_ptr<ExpressionNode> AST::buildTermAST(std::queue<Token>& tokens) {
    std::shared_ptr<ExpressionNode> factorNode = buildFactorAST(tokens);
    return buildSubTermAST(tokens, factorNode);
}

std::shared_ptr<ExpressionNode> AST::buildSubTermAST(std::queue<Token>& tokens, std::shared_ptr<ExpressionNode> node) {
    // reached e, which is the empty string
    if (tokens.size() == 0) {
        return node;
    }
    Token front = tokens.front();
    if (front.type == MUL || front.type == MOD || front.type == DIV) {
        tokens.pop();
        std::shared_ptr<ExpressionNode> factorNode = buildFactorAST(tokens);
        return buildSubTermAST(tokens,
                               std::make_shared<ExpressionNode>(front.type, node, factorNode, front.line_number));
    }

    return node;
}

std::shared_ptr<ExpressionNode> AST::buildFactorAST(std::queue<Token>& tokens) {
    Token token = tokens.front();
    tokens.pop();
    if (token.type == NAME) {
        return buildVarNameAST(token);
    } else if (token.type == INTEGER) {
        return buildIntAST(token);
    }
    return buildExprFromFactorAST(tokens);
}

std::shared_ptr<ExpressionNode> AST::buildExprFromFactorAST(std::queue<Token>& tokens) {
    std::shared_ptr<ExpressionNode> expression = buildExpressionAST(tokens);
    tokens.pop();
    return expression;
}

std::shared_ptr<VariableNode> AST::buildVarNameAST(Token token) {
    return std::make_shared<VariableNode>(VariableNode(token.value, token.line_number));
}

std::shared_ptr<ConstantNode> AST::buildIntAST(Token token) {
    return std::make_shared<ConstantNode>(ConstantNode(token.value, token.line_number));
}
