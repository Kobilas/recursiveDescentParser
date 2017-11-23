/*
* parser.cpp
*
*/

#include <string>
using std::string;

#include "lexer.h"
#include "parser.h"

class ParserToken {
private:
    Token	tok;
    bool	pushedBack;

public:
    ParserToken() : pushedBack(false) {}

    Token getToken(istream *in) {
        if (pushedBack) {
            pushedBack = false;
            return tok;
        }
        return ::getToken(in);
    }
    void pushbackToken(const Token& t) {
        if (pushedBack) error(t.GetLinenum(), "syntax error");
        tok = t;
        pushedBack = true;
    }
} ParserToken;

Token nextToken;
extern int numId, numSet, numStar, numPlus;

void treeTraverse(ParseTree* node) {
    if (node == 0)
        return;
    if (node->getLeft() != 0) {
        std::cout << "L";
        treeTraverse(node->getLeft());
        std::cout << "u";
    }
    if (node->getRight() != 0) {
        std::cout << "R";
        treeTraverse(node->getRight());
        std::cout << "U";
    }
    std::cout << "N";
}

ParseTree * Prog(istream* in)
{
    return StmtList(in);
}

ParseTree * StmtList(istream* in)
{
    ParseTree *stmt = Stmt(in);
    if (stmt == 0)
        return 0;
    return new StatementList(stmt, StmtList(in));
}

ParseTree *	Stmt(istream* in)
{
    ParseTree * statement = Prnt(in);
    if (statement == 0) statement = Decl(in);
    if (statement == 0) statement = Set(in);

    if (statement != 0) {
        nextToken = ParserToken.getToken(in);
        if (nextToken == T_SC) return statement;
        else {
            error(nextToken.GetLinenum(), "Syntax error missing semicolon\n");
            return 0;
        }
    }

    return 0;
}

ParseTree *	Decl(istream* in)
{
    nextToken = ParserToken.getToken(in);
    if (nextToken == T_INT || nextToken == T_STRING) {
        bool isString = false;
        if (nextToken == T_STRING) isString = true;
        nextToken = ParserToken.getToken(in);
        if (nextToken == T_ID) {
            numId++;
            if (isString) return new StringSt(nextToken);
            else return new IntegerSt(nextToken);
        }
        else {
            ParserToken.pushbackToken(nextToken);
            error(nextToken.GetLinenum(), "Syntax error ID required");
            return 0;
        }
    }
    ParserToken.pushbackToken(nextToken);
    return 0;
}

ParseTree *	Set(istream* in)
{
    nextToken = ParserToken.getToken(in);
    if (nextToken.GetTokenType() != T_SET) {
        ParserToken.pushbackToken(nextToken);
        return 0;
    }
    if (nextToken.GetTokenType() == T_SET)
    {
        nextToken = ParserToken.getToken(in);
        if (nextToken != T_ID) {
            ParserToken.pushbackToken(nextToken);
            error(nextToken.GetLinenum(), "Syntax error ID required");
            return 0;
        }
        ParseTree *expr = Expr(in);
        if (expr != 0) {
            numSet++;
            return new SetSt(expr, 0);
        }
    }
    else ParserToken.pushbackToken(nextToken);
    error(nextToken.GetLinenum(), "Syntax error expression required");
    return 0;
}

ParseTree *	Prnt(istream* in)
{
    nextToken = getToken(in);
    if (nextToken.GetTokenType() == T_PRINT || nextToken.GetTokenType() == T_PRINTLN) {
        ParseTree *expr = Expr(in);
        if (expr != 0) {
            return new Print(expr, 0);
        }
    }
    ParserToken.pushbackToken(nextToken);
    return 0;
}

ParseTree *Expr(istream* in)
{
    ParseTree *t1 = Trm(in);
    if (t1 == 0)
        return 0;

    for (;;) {
        Token op = ParserToken.getToken(in);
        if (op != T_PLUS && op != T_MINUS) {
            ParserToken.pushbackToken(op);
            return t1;
        }

        ParseTree *t2 = Expr(in);
        if (t2 == 0) {
            error(op.GetLinenum(), "expression required after + or - operator");
            return 0;
        }

        // combine t1 and t2 together
        if (op == T_PLUS) {
            t1 = new Addition(op.GetLinenum(), t1, t2);
            numPlus++;
        }
        else
            t1 = new Subtraction(op.GetLinenum(), t1, t2);
    }

    // should never get here...
    return 0;
}

ParseTree *	Trm(istream* in)
{
    ParseTree *prim = Primary(in);
    if (prim == 0)
        return 0;

    for (;;) {
        Token op = ParserToken.getToken(in);
        if (op != T_STAR && op != T_SLASH) {
            ParserToken.pushbackToken(op);
            return prim;
        }

        ParseTree *termo = Trm(in);
        if (termo == 0) {
            error(op.GetLinenum(), "expression required after + or - operator");
            return 0;
        }

        // combine t1 and t2 together
        if (op == T_STAR) {
            prim = new Multiplication(op.GetLinenum(), prim, termo);
            numStar++;
        }
        else
            prim = new Division(op.GetLinenum(), prim, termo);
    }

    // should never get here...
    return 0;
}

ParseTree *	Primary(istream* in)
{
    nextToken = ParserToken.getToken(in);
    if (nextToken.GetTokenType() == T_ICONST) return new IntegerConstant(nextToken);
    if (nextToken.GetTokenType() == T_SCONST) return new StringConstant(nextToken);
    if (nextToken.GetTokenType() == T_ID) return new ID(nextToken);
    error(nextToken.GetLinenum(), "Syntax error primary expected");
    return 0;
}