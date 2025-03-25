#ifndef STATICSEMANTICS_H
#define STATICSEMANTICS_H
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include "node.h"

class ST {
public:
    std::vector<std::string> variables;
    std::vector<std::string> used;

public:
    void insert(std::string str, std::string str2);
    bool verify(std::string str);
    void checkVars(std::string str);
};

extern ST STV;

void exitError(std::string s);
void checkUnused();
void checkSemantics(Node* root);

#endif /* STATICSEMANTICS_H */
