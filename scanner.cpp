#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include "scanner.h"
#include "token.h"
#include "node.h"
using namespace std;

bool containsOperators(char ch) {
    set<char> targetLetters = {'+', '~', ':', ';', '-', '/', '%', '(', ')', '{', '}', '[', ']', '=', '*', ','};
    return targetLetters.find(ch) != targetLetters.end();
}

bool containsChar(char ch) {
    set<char> targetLetters = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '_', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z','.'};
    return targetLetters.find(ch) != targetLetters.end();
}

bool containsDigits(char ch) {
    set<char> targetDigits = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    return targetDigits.find(ch) != targetDigits.end();
}

bool isKeyword(string str) {
    set<string> keywords = {"start", "stop", "iterate", "var", "exit", "read", "print", "iff", "then", "set", "func", "program"};
    return keywords.find(str) != keywords.end();
}

bool doesNotContainNull(const std::string& str) {
    for (char c : str) {
        if (c == '\0')
            return false;
    }
    return true;
}

struct token scanner(string s, int* lineNumber, int* x) {
    int i = *x;
    int lineNum = *lineNumber;
    char lookahead;
    string instance = "";
    int state = -1;
    int length = 0;
    bool breakout = false;
    char ch = ' ';

    if (containsDigits(s[i]))
        state = 1;
    if (containsChar(s[i]))
        state = 0;
    if (containsOperators(s[i]))
        state = 2;
    if (s[i] == '@')
        state = 4;

    while (length < 8 && breakout == false) {
        ch = s[i];
        instance += ch;
        lookahead = s[i + 1];
        if (lookahead == '\0')
            lineNum++;

        if (isspace(lookahead) || lookahead == '\0') {
            i++;
            breakout = true;
        }

        if (instance == "@@") {
            i += 2;
            while (s[i] != '@')
                i++;
            instance = "";
            breakout = true;
        }

        switch (state) {
            case 0: //Ident
                if (isspace(lookahead) || containsOperators(lookahead) || lookahead == '@' || lookahead == '\0')
                    breakout = true;
                else if (containsChar(lookahead) || containsDigits(lookahead))
                    i++;
                else {
                    breakout = true;
                    instance += lookahead;
                    state = -1;
                }
                break;
            case 1: //Int
                if (containsChar(lookahead) || isspace(lookahead) || containsOperators(lookahead) || lookahead == '@' || lookahead == '\0')
                    breakout = true;
                else if (containsDigits(lookahead))
                    i++;
                else {
                    breakout = true;
                    instance += lookahead;
                    state = -1;
                }
                break;
            case 2: //Operator
                if (lookahead == '*' && instance.length() < 2)
                    i++;
                else if (containsDigits(lookahead) || isspace(lookahead) || containsChar(lookahead) || containsOperators(lookahead) || lookahead == '@' || lookahead == '\0')
                    breakout = true;
                else
                    i++;
                break;
        }
        length++;
    }

    token newToken;
    switch (state) {
        case 0:
            newToken.id = IDENT_tk;
            break;
        case 1:
            newToken.id = INT_tk;
            break;
        case 2:
            newToken.id = OP_tk;
            break;
    }
    if (isKeyword(instance))
        newToken.id = KW_tk;
    if (instance == ".le." || instance == ".ge." || instance == ".lt." || instance == ".gt.")
        newToken.id = OP_tk;

    newToken.tokenInstance = instance;
    newToken.lineNum = lineNum;

    if (state == -1 && instance.find_first_not_of(' ') != string::npos && doesNotContainNull(instance)) {
        newToken.id = ERROR_tk;
        newToken.tokenInstance = instance;
        newToken.lineNum = lineNum;
    }

    if (i >= (int) s.length()) {
        newToken.id = EOF_tk;
        newToken.tokenInstance = "0";
        newToken.lineNum = -1;
    }

    *x = i;
    *lineNumber = lineNum;
    return newToken;
}
