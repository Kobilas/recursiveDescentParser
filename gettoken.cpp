#include <iostream>
#include <fstream>
#include <map>
#include <cctype>
#include <string>
#include "lexer.h"
using namespace std;
int lineNumber = 0;
map<enum TokenType, string> typeName =
{
    { T_INT,     "T_INT" },
    { T_STRING,  "T_STRING" },
    { T_SET,     "T_SET" },
    { T_PRINT,   "T_PRINT" },
    { T_PRINTLN, "T_PRINTLN" },
    { T_ID,      "T_ID" },
    { T_ICONST,  "T_ICONST" },
    { T_SCONST,  "T_SCONST" },
    { T_PLUS,    "T_PLUS" },
    { T_MINUS,   "T_MINUS" },
    { T_STAR,    "T_STAR" },
    { T_SLASH,   "T_SLASH" },
    { T_LPAREN,  "T_LPAREN" },
    { T_RPAREN,  "T_RPAREN" },
    { T_SC,      "T_SC" },
    { T_ERROR,   "T_ERROR" },
    { T_DONE,    "T_DONE" }
};
ostream& operator<<(ostream& o, const Token& t)
{
    TokenType tt = t.GetTokenType();
    switch (tt)
    {
    case T_ID:
    case T_ICONST:
    case T_SCONST:
    case T_ERROR:  o << typeName[tt] << "(" << t.GetLexeme() << ")"; break;
    default:
        o << typeName[tt]; break;
    }
    return o;
}
enum State
{
    S_NONE,
    S_VAR,
    S_NUM,
    S_STR,
    S_COM
};
string strip(string s, unsigned int n = 1)
{
    int m = s.length() - n;
    if (m<0) m = 0;
    return s.substr(0, m);
}
Token getToken(istream *is)
{
    State s = S_NONE;
    string lex = "";
    int c;
    while ((c = is->get()) != EOF)
    {
        lex += c;
        if (s == S_NONE)
        {
            switch (c)
            {
            case '+': return Token(T_PLUS, lex);
            case '-': return Token(T_MINUS, lex);
            case '*': return Token(T_STAR, lex);
            case '/': s = S_COM; continue;
            case '(': return Token(T_LPAREN, lex);
            case ')': return Token(T_RPAREN, lex);
            case ';': return Token(T_SC, lex);
            case '\n': lineNumber++; lex = ""; continue;
            case '"': s = S_STR; continue;
            default: break;
            }
            if (isalpha(c)) { s = S_VAR; continue; }
            if (isdigit(c)) { s = S_NUM; continue; }
            if (isspace(c)) { lex = "";  continue; }
            return Token(T_ERROR, lex);
        }
        else if (s == S_STR)
        {
            if (c == '"') return Token(T_SCONST, lex);
            if (c == '\n') return Token(T_ERROR, lex);
        }
        else if (s == S_VAR)
        {
            if (isalnum(c)) continue;
            is->putback(c);
            lex = strip(lex);
            if (lex == "int") return Token(T_INT, lex);
            if (lex == "string") return Token(T_STRING, lex);
            if (lex == "set") return Token(T_SET, lex);
            if (lex == "print") return Token(T_PRINT, lex);
            if (lex == "println") return Token(T_PRINTLN, lex);
            return Token(T_ID, lex);
        }
        else if (s == S_NUM)
        {
            if (isdigit(c)) continue;
            is->putback(c);
            lex = strip(lex);
            return Token(T_ICONST, lex);
        }
        else if (s == S_COM)
        {
            if (lex.length() == 2 && lex[1] != '/')
            {
                is->putback(c);
                lex = strip(lex);
                return Token(T_SLASH, lex);
            }
            if (c == '\n')
            {
                s = S_NONE;
                lex = "";
            }
        }
    }
    if (s == S_VAR)
    {
        if (lex == "int") return Token(T_INT, lex);
        if (lex == "string") return Token(T_STRING, lex);
        if (lex == "set") return Token(T_SET, lex);
        if (lex == "print") return Token(T_PRINT, lex);
        if (lex == "println") return Token(T_PRINTLN, lex);
        return Token(T_ID, lex);
    }
    else if (s == S_NUM)
    {
        return Token(T_ICONST, lex);
    }
    else if (s == S_STR)
    {
        return Token(T_ERROR, lex);
    }
    return Token(T_DONE, "");
}