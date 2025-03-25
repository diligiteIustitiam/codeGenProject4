#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <map>
#include "node.h"
#include "codeGeneration.h"
using namespace std;

string globalVars = "";
int labelCntr = 0; /* counting unique labels generated */
int varCntr = 0; /* counting unique temporaries generated */
int relation = 0;

typedef enum {
	VAR, LABEL
} nameType;

map<string, int> nodeMap = { { "program", 0 }, { "vars", 1 }, { "block", 2 }, {
		"varList", 3 }, { "x", 4 }, { "stats", 5 }, { "stat", 6 },
		{ "mStat", 7 }, { "read", 8 }, { "print", 9 }, { "cond", 10 }, { "iter",
				11 }, { "itCondL", 12 }, { "itCondR", 13 }, { "assign", 14 }, {
				"relational", 15 }, { "exp", 16 }, { "m", 17 }, { "n", 18 }, {
				"r", 19 } };

static string newName(nameType what) {
	std::string Name;
	if (what == VAR) {
		Name = "t" + to_string(varCntr++);
		globalVars += Name + " 0\n";
	} else {
		Name = "L" + to_string(labelCntr++);
		globalVars += Name + " 0\n";
	}
	return Name;
}

void recGen(Node *root, ofstream &MyFile) {
	string lastTemp = "";
	string tempVariable = "";
	if (root == nullptr)
		return;
	switch (nodeMap[root->data.front()]) {
	case 0: //program
		recGen(root->left, MyFile);
		recGen(root->right, MyFile);
		break;
	case 1: //vars
		recGen(root->left, MyFile);
		recGen(root->right, MyFile);
		break;
	case 2: //block
		recGen(root->left, MyFile);
		recGen(root->right, MyFile);
		break;
	case 3: //varList
		recGen(root->left, MyFile);
		MyFile << "LOAD ";
		MyFile << root->data[5] << endl;
		MyFile << "STORE ";
		MyFile << root->data[2] << endl;
		recGen(root->right, MyFile);
		break;
	case 4: //x
		recGen(root->left, MyFile);
		recGen(root->right, MyFile);
		break;
	case 5: //stats
		recGen(root->left, MyFile);
		recGen(root->right, MyFile);
		break;
	case 6: //stat
		recGen(root->left, MyFile);
		recGen(root->right, MyFile);
		break;
	case 7: //mStats
		recGen(root->left, MyFile);
		recGen(root->right, MyFile);
		break;
	case 8: //read
		recGen(root->left, MyFile);
		MyFile << "READ ";
		MyFile << root->data[2] << endl;
		recGen(root->right, MyFile);
		break;
	case 9: //print
		recGen(root->left, MyFile);
		tempVariable = newName(VAR);
		MyFile << "STORE " << tempVariable << endl;
		MyFile << "WRITE " << tempVariable << endl;
		recGen(root->right, MyFile);
		break;
	case 10: //cond
		recGen(root->left, MyFile);
		if (relation == 1)
			MyFile << "BRZPOS ";
		if (relation == 2)
			MyFile << "BRPOS ";
		if (relation == 3)
			MyFile << "BRZNEG ";
		if (relation == 4)
			MyFile << "BRNEG ";
		if (relation == 5)
			MyFile << "BRPOS ";
		if (relation == 6)
			MyFile << "BRZERO ";
		tempVariable = newName(LABEL);
		MyFile << tempVariable << endl;
		if (relation == 5) {
			MyFile << "BRNEG ";
			MyFile << tempVariable << endl;
		}
		relation = 0;
		recGen(root->right, MyFile);
		MyFile << tempVariable << ": NOOP" << endl;
		break;
	case 11: //iter
		tempVariable = newName(LABEL);
		lastTemp = tempVariable;
		MyFile << tempVariable << ": ";
		recGen(root->left, MyFile);
		if (relation == 1)
			MyFile << "BRZPOS ";
		if (relation == 2)
			MyFile << "BRPOS ";
		if (relation == 3)
			MyFile << "BRZNEG ";
		if (relation == 4)
			MyFile << "BRNEG ";
		if (relation == 5)
			MyFile << "BRPOS ";
		if (relation == 6)
			MyFile << "BRZERO ";
		tempVariable = newName(LABEL);
		MyFile << tempVariable << endl;
		if (relation == 5) {
			MyFile << "BRNEG ";
			MyFile << tempVariable << endl;
		}
		relation = 0;
		recGen(root->right, MyFile);
		MyFile << "BR " << lastTemp << endl;
		MyFile << tempVariable << ": NOOP" << endl;
		break;
	case 12: //itCondL
		recGen(root->left, MyFile);
		tempVariable = newName(VAR);
		MyFile << "STORE " << tempVariable << endl;
		recGen(root->right, MyFile);
		MyFile << "SUB " << tempVariable << endl;
		break;
	case 13: //itCondR
		recGen(root->right, MyFile);
		recGen(root->left, MyFile);
		break;
	case 14: //assign
		recGen(root->left, MyFile);
		recGen(root->right, MyFile);
		MyFile << "STORE " << root->data[2] << endl;
		break;
	case 15: //relational
		if (root->data[2] == ".gt.")
			relation = 1;
		if (root->data[2] == ".ge.")
			relation = 2;
		if (root->data[2] == ".lt.")
			relation = 3;
		if (root->data[2] == ".le.")
			relation = 4;
		if (root->data[2] == "**")
			relation = 5;
		if (root->data[2] == "~")
			relation = 6;
		recGen(root->left, MyFile);
		recGen(root->right, MyFile);
		break;
	case 16: //exp
		if (root->data.size() > 2) {
			if (root->data[2] == "+") {
				recGen(root->left, MyFile);
				tempVariable = newName(VAR);
				MyFile << "STORE " << tempVariable << endl;
				recGen(root->right, MyFile);
				MyFile << "ADD " << tempVariable << endl;
			}
			if (root->data[2] == "-") {
				recGen(root->right, MyFile);
				tempVariable = newName(VAR);
				MyFile << "STORE " << tempVariable << endl;
				recGen(root->left, MyFile);
				MyFile << "SUB " << tempVariable << endl;
			}
		} else
			recGen(root->left, MyFile);
		break;
	case 17: //m
		if (root->data.size() > 2) {
			if (root->data[2] == "%") {
				recGen(root->left, MyFile);
				tempVariable = newName(VAR);
				MyFile << "STORE " << tempVariable << endl;
				recGen(root->right, MyFile);
				MyFile << "MULT " << tempVariable << endl;
			}
		} else
			recGen(root->left, MyFile);
		break;
	case 18: //n
		if (root->data.size() > 2) {
			if (root->data[2] == "-") {
				recGen(root->left, MyFile);
				MyFile << "MULT -1" << endl;
			}
		}
		if (root->data.size() > 2) {
			if (root->data[2] == "/") {
				recGen(root->right, MyFile);
				tempVariable = newName(VAR);
				MyFile << "STORE " << tempVariable << endl;
				recGen(root->left, MyFile);
				MyFile << "DIV " << tempVariable << endl;
			}
		} else
			recGen(root->left, MyFile);
		break;
	case 19: //r
		//Should load what's in the accumulators
		if (root->data[1] != "Delimiter") {
			MyFile << "LOAD ";
			MyFile << root->data[2] << endl;
		} else
			recGen(root->left, MyFile);
		break;
	}
}
