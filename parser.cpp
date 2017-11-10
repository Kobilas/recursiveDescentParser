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
        ParseTree *print = Prnt(in);
        return new Statement(print, Stmt(in));
    }
    else ParserToken.pushbackToken(nextToken);
    return 0;
}

ParseTree *	Decl(istream* in)
{
    nextToken = ParserToken.getToken(in);
    if (nextToken.GetTokenType() == T_ID)
    {
        ParseTree *decl = new ParseTree(nextToken.GetLinenum());
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

ParseTree *	Prnt(istream* in)
{
    ParseTree *expr = Expr(in);
    return new Print(expr, Prnt(in));
}

ParseTree *Expr(istream* in)
{
    ParseTree *term1 = Trm(in);
    if (term1 == 0) return 0;
    for (;;)
    {
        nextToken = ParserToken.getToken(in);
        if ((nextToken.GetTokenType() != T_PLUS)
            && (nextToken.GetTokenType() != T_MINUS))
        {
            return new Expression(term1, Expr(in));
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

ParseTree *	Trm(istream* in)
{
    ParseTree *primary = Primary(in);
    if (primary == 0)
    {
        return 0;
    }
    for (;;)
    {
        std::cerr << nextToken.GetLexeme();
        nextToken = ParserToken.getToken(in);
        if ((nextToken.GetTokenType() != T_STAR)
            && (nextToken.GetTokenType() != T_SLASH))
        {
            return new Term(primary, 0);
        }
        ParseTree *term = Trm(in);
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
    std::cerr << "primary" << nextToken.GetLexeme();
    nextToken = ParserToken.getToken(in);
    if (nextToken.GetTokenType() == T_ICONST)
    {
        ParseTree *iconst = new IntegerConstant(nextToken);
        return iconst;
    }
    //if (nextToken.GetTokenType() == T_SCONST) return StringConstant(nextToken);
    //if (nextToken.GetTokenType() == T_ID) return 
    return 0;
}