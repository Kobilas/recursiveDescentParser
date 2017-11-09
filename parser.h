/*
* parser.h
*
*  Created on: Oct 23, 2017
*      Author: gerardryan
*/

#ifndef PARSER_H_
#define PARSER_H_

#include <iostream>
using std::istream;
#include <string>
using std::string;
using std::stoi;

#include "lexer.h"

extern void error(int linenum, const string& message);

enum TypeForNode { INT_TYPE, STRING_TYPE, ERROR_TYPE };

class ParseTree {
    int			linenumber;
    ParseTree	*left;
    ParseTree	*right;

public:
    ParseTree(int n, ParseTree *l = 0, ParseTree *r = 0) : linenumber(n), left(l), right(r) {}
    virtual ~ParseTree() {}

    ParseTree* getLeft() const { return left; }
    ParseTree* getRight() const { return right; }
    int getLineNumber() const { return linenumber; }

    virtual TypeForNode GetType() const { return ERROR_TYPE; }
    virtual int GetIntValue() const { throw "no integer value"; }
    virtual string GetStringValue() const { throw "no string value"; }
};

class StatementList : public ParseTree {
public:
    StatementList(int line, ParseTree *first, ParseTree *rest) : ParseTree(0, first, rest) {}
};

class Addition : public ParseTree {
    TypeForNode nodeType;
    int iValue;
    string sValue;
public:
    Addition(int line, ParseTree *op1, ParseTree *op2) : ParseTree(line, op1, op2)
    {
        if ((op1.GetType() == INT_TYPE) && (op2.GetType() == INT_TYPE))
        {
            nodeType = INT_TYPE;
            iValue = op1.GetIntValue() + op2.GetIntValue();
        }
        else if ((op1.GetType() == STRING_TYPE) && (op2.GetType() == STRING_TYPE))
        {
            nodeType = STRING_TYPE;
            sValue = op1.GetStringValue() + op2.GetStringValue();
        }
        else nodeType = ERROR_TYPE;
    }
    TypeForNode GetType() const {return nodeType;}
    int GetIntValue() const
    {
        if (nodeType == INT_TYPE) return iValue;
        else throw "no integer value";
    }
    string GetStringValue() const 
    {
        if (nodeType == STRING_TYPE) return sValue;
        else throw "no string value";
    }
};

class Subtraction : public ParseTree {
    TypeForNode nodeType;
    int value;
public:
    Subtraction(int line, ParseTree *op1, ParseTree *op2) : ParseTree(line, op1, op2)
    {
        if ((op1.GetType() == INT_TYPE) && (op2.GetType() == INT_TYPE))
        {
            nodeType = INT_TYPE;
            value = op1.GetIntValue() - op2.GetIntValue();
        }
        else nodeType = ERROR_TYPE;
    }
    TypeForNode GetType() const {return nodeType;}
    int GetIntValue() const
    {
        if (nodeType == INT_TYPE) return iValue;
        else throw "no integer value";
    }
};

class Multiplication : public ParseTree {
    TypeForNode nodeType;
    int iValue;
    string sValue;
public:
    Multiplication(int line, ParseTree *op1, ParseTree *op2) : ParseTree(line, op1, op2)
    {
        if ((op1.GetType() == INT_TYPE) && (op2.GetType() == INT_TYPE))
        {
            nodeType = INT_TYPE;
            iValue = op1.GetIntValue() * op2.GetIntValue();
        }
        else if ((op1.GetType() == INT_TYPE) && (op2.GetType() == STRING_TYPE))
        {
            nodeType = STRING_TYPE;
            sValue = "";
            int max = op1.GetIntValue();
            string str = op2.GetStringValue();
            for(int i = 0; i < max; i++) sValue += str;
        }
        else if ((op1.GetType() == STRING_TYPE) && (op2.GetType() == INT_TYPE))
        {
            nodeType = STRING_TYPE;
            sValue = "";
            int max = op2.GetIntValue();
            string str = op1.GetStringValue();
            for(int i = 0; i < max; i++) sValue += str;
        }
        else nodeType = ERROR_TYPE;
    }
    TypeForNode GetType() const {return nodeType;}
    int GetIntValue() const
    {
        if (nodeType == INT_TYPE) return iValue;
        else throw "no integer value";
    }
    string GetStringValue() const
    {
        if (nodeType == STRING_TYPE) return sValue;
        else throw "no string value";
    }
};

class Division : public ParseTree {
    TypeForNode nodeType;
    int iValue;
    string sValue;
public:
    Division(int line, ParseTree *op1, ParseTree *op2) : ParseTree(line, op1, op2)
    {
        if ((op1.GetType() == INT_TYPE) && (op2.GetType() == INT_TYPE))
        {
            nodeType = INT_TYPE;
            iValue = op1.GetIntValue() / op2.GetIntValue();
        }
        else if ((op1.GetType() == STRING_TYPE) && (op2.GetType() == STRING_TYPE))
        {
            nodeType = STRING_TYPE;
            string str = op1.GetStringValue();
            string toFind = op2.GetStringValue();
            sValue = str.replace(str.find(toFind), toFind.length(), "");
        }
        else nodeType = ERROR_TYPE;
    }
    TypeForNode GetType() const {return nodeType;}
    int GetIntValue() const
    {
        if (nodeType == INT_TYPE) return iValue;
        else throw "no integer value";
    }
    string GetStringValue() const
    {
        if (nodeType == STRING_TYPE) return sValue;
        else throw "no string value";
    }
};

class IntegerConstant : public ParseTree {
    int	value;
public:
    IntegerConstant(const Token& tok) : ParseTree(tok.GetLinenum())
    {
        value = stoi(tok.GetLexeme());
    }
    TypeForNode GetType() const {return INT_TYPE;}
    int GetIntValue() const {return value;}
};

class StringConstant : public ParseTree {
    string value;
public:
    StringConstant(const Token& tok) : ParseTree(tok.GetLinenum())
    {
        value = tok.GetLexeme();
    }
    TypeForNode GetType() const {return STRING_TYPE;}
    int GetStringValue() const {return value;}
};

extern ParseTree *	Prog(istream* in);
extern ParseTree *	StmtList(istream* in);
extern ParseTree *	Stmt(istream* in);
extern ParseTree *	Decl(istream* in);
extern ParseTree *	Set(istream* in);
extern ParseTree *	Print(istream* in);
extern ParseTree *	Expr(istream* in);
extern ParseTree *	Term(istream* in);
extern ParseTree *	Primary(istream* in);


#endif /* PARSER_H_ */
