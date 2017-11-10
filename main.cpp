#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include "lexer.h"
#include "parser.h"
using namespace std;
void error(int linenum, const string& message)
{
    cout << linenum + 1 << ":" << message;
}
bool OPT_V = false, OPT_I = false, OPT_S = false, OPT_T = false;
bool endToken(Token& t)
{
    if (t.GetTokenType() == T_ERROR) return true;
    if (t.GetTokenType() == T_DONE) return true;
    return false;
}
int main(int argc, char *argv[])
{
    for (int i = 1; i<argc; i++)
    {
        if (argv[i][0] != '-') continue;
        string arg(argv[i]);
        if (arg == "-v") OPT_V = true;
        else if (arg == "-i") OPT_I = true;
        else if (arg == "-s") OPT_S = true;
        else if (arg == "-t") OPT_T = true;
        else
        {
            cout << arg << " UNRECOGNIZED FLAG" << endl;
            return 1;
        }
    }
    bool foundfile = false;
    istream *in = &cin;
    ifstream file;
    for (int i = 1; i<argc; i++)
    {
        if (argv[i][0] == '-') continue;
        if (foundfile)
        {
            cout << "TOO MANY FILES" << endl;
            return 1;
        }
        foundfile = true;
        string arg(argv[i]);
        file.open(arg);
        if (!file.is_open())
        {
            cout << arg << " FILE NOT FOUND" << endl;
            return 1;
        }
        in = &file;
    }
    /*
    Token t;
    map<string, unsigned int> smap, imap;
    while (!endToken(t = getToken(in)))
    {
        if (OPT_V) cout << t << endl;
        if (t.GetTokenType() == T_SCONST) { smap[t.GetLexeme()]++; }
        if (t.GetTokenType() == T_ID) { imap[t.GetLexeme()]++; }
    }
    if (t.GetTokenType() == T_ERROR)
    {
        cout << "Lexical error " << t << endl;
        return 1;
    }
    if (OPT_S)
    {
        map<string, unsigned int>::iterator i;
        bool first = true;
        for (i = smap.begin(); i != smap.end(); i++)
        {
            if (!first) cout << ", ";
            first = false;
            cout << i->first;
        }
        if (!first) cout << endl;
    }
    if (OPT_I)
    {
        map<string, unsigned int>::iterator i;
        bool first = true;
        for (i = imap.begin(); i != imap.end(); i++)
        {
            if (!first) cout << ", ";
            first = false;
            cout << i->first;
        }
        if (!first) cout << endl;
    }
    */
    ParseTree *tree = Prog(in);
    if (tree == 0)
    {
        return 1;
    }
    tree->nodeCount(OPT_T);
    cout << endl << "Total number of identifiers: " << tree->GetNumId() << endl;
    cout << "Total number of set: " << tree->GetNumSet() << endl;
    cout << "Total number of +: " << tree->GetNumPlus() << endl;
    cout << "Total number of *: " << tree->GetNumStar() << endl;
    return 0;
}
