#include <iostream>
#include <istream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include "parser.h"

using namespace std;

/*
* main.cpp
*/

int lineNumber;

void error(int linenum, const string& message) {
    cout << linenum + 1 << ":" << message;
}

extern Token getToken(istream* br)
{
    map<string, TokenType> tknTypMap = {
        { "int", T_INT },
        { "string", T_STRING },
        { "set", T_SET },
        { "print", T_PRINT },
        { "println", T_PRINTLN },
        { "+", T_PLUS },
        { "-", T_MINUS },
        { "*", T_STAR },
        { "/", T_SLASH },
        { "(", T_LPAREN },
        { ")", T_RPAREN },
        { ";", T_SC }
    };
    char c;
    string lexeme = "";
    bool numFlag = false;
    bool idFlag = false;
    bool stringFlag = false;
    bool firstFlag = true;
    int lineNumber = 1;
    while ((c = br->get()))
    {
        if (c == '\n')
        {
            lineNumber++;
            if (stringFlag)
            {
                lexeme += c;
                return Token(T_ERROR, lexeme);
            }
        }
        if (firstFlag)
        {
            if (isspace(c))
                continue;
            switch (c) {
            case '+':
                return Token(T_PLUS, "");
            case '-':
                return Token(T_MINUS, "");
            case '*':
                return Token(T_STAR, "");
            case '/':
                return Token(T_SLASH, "");
            case '(':
                return Token(T_LPAREN, "");
            case ')':
                return Token(T_RPAREN, "");
            case ';':
                return Token(T_SC, "");
            default:
                break;
            }
            if (c == '\"')
            {
                stringFlag = true;
                lexeme += c;
                firstFlag = false;
                continue;
            }
            if (isalpha(c))
                idFlag = true;
            if (isdigit(c))
                numFlag = true;
            firstFlag = false;
        }
        if (numFlag && isdigit(c))
            lexeme += c;
        else if (numFlag)
        {
            br->putback(c);
            return Token(T_ICONST, lexeme);
        }
        if (idFlag && isalnum(c))
            lexeme += c;
        else if (idFlag)
        {
            br->putback(c);
            if (tknTypMap.count(lexeme) == 1)
                return Token(tknTypMap[lexeme], "");
            return Token(T_ID, lexeme);
        }
        if (stringFlag && (c == '\"'))
        {
            lexeme += c;
            return Token(T_SCONST, lexeme);
        }
        else if (stringFlag)
        {
            lexeme += c;
            continue;
        }
        if (br->eof())
            return Token(T_DONE, "");
        if (c == '.')
        {
            lexeme += c;
            return Token(T_ERROR, lexeme);
        }
        if (!isalnum(c))
        {
            lexeme += c;
            return Token(T_DONE, "");
        }
    }
    return Token(T_DONE, "");
}

extern ostream& operator<<(ostream& out, const Token& tok) {
    map<TokenType, string> tknTypMap = {
        { T_INT, "T_INT" },
        { T_STRING, "T_STRING" },
        { T_SET, "T_SET" },
        { T_PRINT, "T_PRINT" },
        { T_PRINTLN, "T_PRINTLN" },
        { T_PLUS, "T_PLUS" },
        { T_MINUS, "T_MINUS" },
        { T_STAR, "T_STAR" },
        { T_SLASH, "T_SLASH" },
        { T_LPAREN, "T_LPAREN" },
        { T_RPAREN, "T_RPAREN" },
        { T_SC, "T_SC" },
        { T_ERROR, "T_ERROR" },
        { T_ID, "T_ID" },
        { T_SCONST, "T_SCONST" },
        { T_ICONST, "T_ICONST" }
    };
    if (tok.GetTokenType() == T_ID || tok.GetTokenType() == T_SCONST || tok.GetTokenType() == T_ICONST || tok.GetTokenType() == T_ERROR)
    {
        out << tknTypMap[tok.GetTokenType()] << "(" << tok.GetLexeme() << ")";
        return out;
    }
    out << tknTypMap[tok.GetTokenType()];
    return out;
}

int main(int argc, char *argv[])
{
    ifstream inFile;
    stringstream inString;
    string input = "";
    string buffer = "";
    bool sFlag = false;
    bool iFlag = false;
    bool vFlag = false;
    bool tFlag = false;
    bool slashFlag = false;
    bool firstLoop = true;
    int fileCounter = 0;
    int index = -1;
    int numId = 0;
    int numSet = 0;
    int numPlus = 0;
    int numStar = 0;
    int lineNumber = 1;
    Token gotToken;
    vector<string> strings;
    vector<string> identifiers;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
            case 's':
                sFlag = true;
                break;
            case 'i':
                iFlag = true;
                break;
            case 'v':
                vFlag = true;
                break;
            case 't':
                tFlag = true;
                break;
            default:
                cout << argv[i] << " UNRECOGNIZED FLAG" << endl;
                return 0;
            }
        }
        else
        {
            fileCounter++;
            index = i;
        }
    }
    if (fileCounter > 0)
    {
        if (fileCounter > 1)
        {
            cout << "TOO MANY FILES" << endl;
            return 0;
        }
        inFile.open(argv[index]);
        if (!inFile.is_open())
        {
            cout << argv[index] << " FILE NOT FOUND" << endl;
            return 0;
        }
        gotToken = getToken(&inFile);
        while (!(gotToken.GetTokenType() == T_DONE || gotToken.GetTokenType() == T_ERROR))
        {
            if (gotToken.GetTokenType() == T_SLASH)
            {
                if (slashFlag == true)
                {
                    inFile.ignore(256, '\n');
                    slashFlag = false;
                }
                slashFlag = true;
            }
            if (slashFlag && gotToken.GetTokenType() != T_SLASH)
            {
                slashFlag = false;
                if (vFlag)
                    cout << "T_SLASH" << endl;
            }
            if (vFlag && gotToken.GetTokenType() != T_SLASH)
                cout << gotToken << endl;
            if (gotToken.GetTokenType() == T_ID)
                numId++;
            else if (gotToken.GetTokenType() == T_SET)
                numSet++;
            else if (gotToken.GetTokenType() == T_PLUS)
                numPlus++;
            else if (gotToken.GetTokenType() == T_STAR)
                numStar++;
            if (sFlag && gotToken.GetTokenType() == T_SCONST)
                strings.push_back(gotToken.GetLexeme());
            if (iFlag && gotToken.GetTokenType() == T_ID)
                identifiers.push_back(gotToken.GetLexeme());
            gotToken = getToken(&inFile);
        }
    }
    else
    {
        while (getline(cin, input))
        {
            inString << " " << input;
        }
        gotToken = getToken(&inString);
        while (!(gotToken.GetTokenType() == T_DONE || gotToken.GetTokenType() == T_ERROR))
        {
            if (gotToken.GetTokenType() == T_SLASH)
            {
                if (slashFlag == true)
                {
                    inString.ignore(256, '\n');
                    slashFlag = false;
                }
                slashFlag = true;
            }
            if (slashFlag && gotToken.GetTokenType() != T_SLASH)
            {
                slashFlag = false;
                if (vFlag)
                    cout << "T_SLASH" << endl;
            }
            if (vFlag && gotToken.GetTokenType() != T_SLASH)
                cout << gotToken << endl;
            if (gotToken.GetTokenType() == T_ID)
                numId++;
            else if (gotToken.GetTokenType() == T_SET)
                numSet++;
            else if (gotToken.GetTokenType() == T_PLUS)
                numPlus++;
            else if (gotToken.GetTokenType() == T_STAR)
                numStar++;
            if (sFlag && gotToken.GetTokenType() == T_SCONST)
                strings.push_back(gotToken.GetLexeme());
            if (iFlag && gotToken.GetTokenType() == T_ID)
                identifiers.push_back(gotToken.GetLexeme());
            gotToken = getToken(&inString);
        }
    }
    if (gotToken.GetTokenType() == T_ERROR)
    {
        cout << "Lexical error " << gotToken << endl;
        return 0;
    }
    lineNumber = gotToken.GetLinenum();
    if (lineNumber < 1)
        lineNumber++;
    cout << "Total number of identifiers: " << numId << endl;
    cout << "Total number of set: " << numSet << endl;
    cout << "Total number of +: " << numPlus << endl;
    cout << "Total number of *: " << numStar << endl;
    if (sFlag)
    {
        sort(strings.begin(), strings.end());
        firstLoop = true;
        for (vector<string>::iterator sit = strings.begin(); sit != strings.end(); ++sit)
        {
            if (firstLoop)
            {
                cout << *sit;
                firstLoop = false;
            }
            else
            {
                cout << ", ";
                cout << *sit;
            }
        }
        if (!strings.empty())
            cout << endl;
    }
    if (iFlag)
    {
        sort(identifiers.begin(), identifiers.end());
        firstLoop = true;
        for (vector<string>::iterator iit = identifiers.begin(); iit != identifiers.end(); ++iit)
        {
            if (firstLoop)
            {
                cout << *iit;
                firstLoop = false;
            }
            else
            {
                cout << ", ";
                cout << *iit;
            }
        }
        if (!identifiers.empty())
            cout << endl;
    }
    return 0;
    ParseTree *tree = Prog(&cin);
    if (tree == 0) {
        // there was some kind of parse error
        return 1;
    }
    return 0;
}