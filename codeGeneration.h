#ifndef CODEGENERATION_H_
#define CODEGENERATION_H_
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <map>
#include "node.h"

extern std::string globalVars;

void recGen(Node *root, std::ofstream& MyFile);

#endif /* CODEGENERATION_H_ */
