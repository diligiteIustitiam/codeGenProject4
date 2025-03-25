#ifndef SCANNER_H
#define SCANNER_H
#include <string>

bool containsOperators(char ch);
bool containsChar(char ch);
bool containsDigits (char ch);
bool isKeyword(std::string str);
struct token scanner(std::string s, int* lineNumber, int* i);

#endif /* SCANNER_H */
