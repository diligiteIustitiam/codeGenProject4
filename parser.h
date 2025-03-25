#ifndef PARSER_H
#define PARSER_H
#include <string>
#include "token.h"
#include "node.h"

token getToken(std::string fullInput, int* lineNumber, int* i);
bool doesNotContainNull(std::string str);

Node* parser(std::string fullFileName);
Node* parser();

Node* program(std::string fullInput, int* lineNumber, int* i);
Node* vars(std::string fullInput, int* lineNumber, int* i);
Node* block(std::string fullInput, int* lineNumber, int* i);
Node* varList(std::string fullInput, int* lineNumber, int* i);
Node* x(std::string fullInput, int* lineNumber, int* i);
Node* stats(std::string fullInput, int* lineNumber, int* i);
Node* stat(std::string fullInput, int* lineNumber, int* i);
Node* mStat(std::string fullInput, int* lineNumber, int* i);
Node* read(std::string fullInput, int* lineNumber, int* i);
Node* print(std::string fullInput, int* lineNumber, int* i);
Node* cond(std::string fullInput, int* lineNumber, int* i);
Node* iter(std::string fullInput, int* lineNumber, int* i);
Node* itCondL(std::string fullInput, int* lineNumber, int* i);
Node* itCondR(std::string fullInput, int* lineNumber, int* i);
Node* assign(std::string fullInput, int* lineNumber, int* i);
Node* relational(std::string fullInput, int* lineNumber, int* i);
Node* exp(std::string fullInput, int* lineNumber, int* i);
Node* m(std::string fullInput, int* lineNumber, int* i);
Node* n(std::string fullInput, int* lineNumber, int* i);
Node* r(std::string fullInput, int* lineNumber, int* i);
#endif /* PARSER_H */
