#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include "staticSemantics.h"
using namespace std;

ST STV;

void exitError(string s) {
    cout << "ERROR - " << s << endl;
    exit(1);
}

void ST::insert(string str, string str2) {
    if (find(variables.begin(), variables.end(), str) != variables.end()) {
        exitError("In static semantics, '" + str + "' variable has multiple redundant declarations on line " + str2);
    } else {
        variables.push_back(str);
        used.push_back(str);
    }
}

bool ST::verify(string str) {
    if (find(variables.begin(), variables.end(), str) != variables.end()) {
        return true;
    } else {
        return false;
    }
}

void ST::checkVars(string str) {
    if (find(used.begin(), used.end(), str) != used.end()) {
        used.erase(find(used.begin(), used.end(), str));
    }
}

void checkUnused() {
    if (STV.used.size() != 0) {
        cout << "WARNING - In static semantics, variables ";
        for (size_t i = 0; i < STV.used.size(); ++i) {
            cout << STV.used[i] << " ";
        }
        cout << "have not been used." << endl;
    }
}

void checkSemantics(Node* root) {
    if (root == nullptr)
        return;
    if (root->data.front() == "varList") {
        for (size_t i = 0; i < root->data.size(); ++i) {
            if (root->data[i] == "ID") {
                STV.insert(root->data[i + 1], root->data[i + 2]);
            }
        }
    }
    if (root->data.front() == "r" || root->data.front() == "read" || root->data.front() == "assign") {
        for (size_t i = 0; i < root->data.size(); ++i) {
            if (root->data[i] == "ID") {
                if (STV.verify(root->data[i + 1])) {
                    STV.checkVars(root->data[i + 1]);
                } else
                    exitError("In static semantics, variable '" + root->data[i + 1] + "' has not yet been declared on line " + root->data[i + 2]);
            }
        }
    }
    checkSemantics(root->left);
    checkSemantics(root->right);
}
