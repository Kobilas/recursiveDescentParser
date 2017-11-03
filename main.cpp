#include <iostream>
#include <string>
using namespace std;

#include "parser.h"

/*
* main.cpp
*/

int lineNumber;

void error(int linenum, const string& message) {
    cout << linenum + 1 << ":" << message;
}


int
main(int argc, char *argv[])
{

    ParseTree *tree = Prog(&cin);

    if (tree == 0) {
        // there was some kind of parse error
        return 1;
    }

    return 0;
}


