/*
* parser.cpp
*
*/

#include <string>
using std::string;

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
        if (pushedBack) {
            extern error(t.GetLinenum(), "syntax error");
        }
        tok = t;
        pushedBack = true;
    }
} ParserToken;

Token nextToken;

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
    nextToken = ParserToken.getToken(in);
    if ((nextToken.GetTokenType() == T_INT)
    || (nextToken.GetTokenType() == T_STRING))
    {
        ParseTree *decl = Decl(in);
        return decl;
    }
    else if (nextToken.GetTokenType() == T_SET)
    {
        ParseTree *set = Set(in);
        return set;
    }
    else if ((nextToken.GetTokenType() == T_PRINT)
    || (nextToken.GetTokenType() == T_PRINTLN))
    {
        ParseTree *print = Print(in);
        return print;
    }
    else ParserToken.pushbackToken(nextToken);
    return 0;
}

ParseTree *	Decl(istream* in)
{
    nextToken = ParserToken.getToken(in);
    if (nextToken.GetTokenType() == T_ID)
    {
        ParseTree *decl = ParseTree(nextToken.GetLinenum());
        return decl;
    }
    else ParserToken.pushbackToken(nextToken);
    return 0;
}

ParseTree *	Set(istream* in)
{
    nextToken = ParserToken.getToken(in);
    if (nextToken.GetTokenType() == T_ID)
    {
        ParseTree *expr = Expr(in);
        return expr;
    }
    else ParserToken.pushbackToken(nextToken);
    return 0;
}

ParseTree *	Print(istream* in)
{
    ParseTree *expr = Expr(in);
    return expr;
}

ParseTree *Expr(istream* in)
{
    ParseTree *term1 = Term(in);
    if (term1 == 0) return 0;
    while (true)
    {
        nextToken = ParserToken.getToken(in);
        if ((nextToken.GetTokenType() != T_PLUS)
        && (nextToken.getTokenType() != T_MINUS))
        {
            ParserToken.pushbackToken(nextToken);
            return term1;
        }
        ParseTree *term2 = Expr(in);
        if (term2 == 0)
        {
            error(nextToken.GetLinenum(), "expression required after + or - operator");
            return 0;
        }
        if (nextToken.GetTokenType() == T_PLUS)
            term1 = new Addition(nextToken.GetLinenum(), term1, term2);
        else term1 = new Subtraction(nextToken.GetLinenum(), term1, term2);
    }
    return 0;
}

ParseTree *	Term(istream* in)
{
    ParseTree *primary = Primary(in);
    if (primary == 0) return 0;
    while (true)
    {
        nextToken = ParserToken.getToken(in);
        if ((nextToken.GetTokenType() != T_STAR)
        && (nextToken.GetTokenType() != T_SLASH))
        {
            ParserToken.pushbackToken(nextToken);
            return primary;
        }
        ParseTree *term = Term(in);
        if (term == 0)
        {
            error(nextToken.GetLinenum(), "expression required after * or / operator");
            return 0;
        }
        if (nextToken.GetTokenType() == T_STAR)
            primary = new Multiplication(nextToken.GetLinenum(), primary, term);
        else primary = new Division(nextToken.GetLinenum(), primary, term);
    }
    return 0;
}

ParseTree *	Primary(istream* in)
{
    if (nextToken.GetTokenType() == T_ICONST) return IntegerConstant(nextToken);
    if (nextToken.GetTokenType() == T_SCONST) return StringConstant(nextToken);
    if (nextToken.GetTokenType() == T_ID) return 
}