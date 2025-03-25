#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include "parser.h"
#include "scanner.h"
#include "token.h"
#include "node.h"
using namespace std;
static token newToken;
static token prevToken;

static void exitError(string s) {
	cout << "ERROR - " << s << endl;
	exit(1);
}

map<tokenID, string> tokenMap = { { IDENT_tk, "IDENT_tk" }, { KW_tk, "KW_tk" },
		{ OP_tk, "OP_tk" }, { INT_tk, "INT_tk" }, { EOF_tk, "EOF_tk" } };

string getTokenString(const map<tokenID, string> &map, tokenID id) {
	auto it = map.find(id);
	if (it != map.end()) {
		return it->second; //Return the corresponding string
	} else {
		return "Unknown token"; //Handle case where the tokenID is not found
	}
}

token getToken(string fullInput, int *lineNumber, int *i) {
	token aToken;
	aToken = scanner(fullInput, lineNumber, i);
	*i += 1;
	if (aToken.tokenInstance.find_first_not_of(' ') == std::string::npos
			|| !doesNotContainNull(aToken.tokenInstance)) {
		while (true) {
			aToken = scanner(fullInput, lineNumber, i);
			*i += 1;
			if (aToken.tokenInstance.find_first_not_of(' ') != std::string::npos
					&& doesNotContainNull(aToken.tokenInstance))
				break;
		}
	}
	return aToken;
}

bool doesNotContainNull(string str) {
	for (char c : str) {
		if (c == '\0') {
			return false; // Found a null character
		}
	}
	return true; // No null characters found
}

void BinaryTree::insert(const string &str) {
	if (root == nullptr) {
		root = new Node();
	}
	insertHelper(root, str);
}

void BinaryTree::insertHelper(Node *node, const string &str) {
	node->data.push_back(str);
	if (str == "") {
		return;
	}
}

Node* parser(string fullFileName) {
	string s;
	int lineNumber = 1;
	string fullInput = "";
	int i = 0;
	string textFromFile;
	ifstream readThis(fullFileName);
	Node *tree = new Node();

	while (getline(readThis, s)) {
		fullInput += s;
		fullInput += '\0';
	}
	readThis.close();

	newToken = getToken(fullInput, &lineNumber, &i);
	tree = program(fullInput, &lineNumber, &i);
	if (newToken.id == ERROR_tk)
		cout << "Error - " << newToken.tokenInstance
				<< " contains a character that doesn't exist on line "
				<< newToken.lineNum << endl;

	if (newToken.id != EOF_tk) {
		exitError("Program should not continue after outermost 'stop' keyword");
	}
	return tree;
}

Node* parser() {
	string s;
	int lineNumber = 1;
	string fullInput = "";
	int i = 0;
	Node *tree = new Node();

	while (getline(cin, s)) {
		fullInput += s;
		fullInput += '\0';
	}

	newToken = getToken(fullInput, &lineNumber, &i);
	tree = program(fullInput, &lineNumber, &i);
	if (newToken.id == ERROR_tk)
		cout << "Error - " << newToken.tokenInstance
				<< " contains a character that doesn't exist on line "
				<< newToken.lineNum << endl;

	if (newToken.id != EOF_tk) {
		exitError("Program should not continue after 'stop' keyword");
	}
	return tree;
}

Node* program(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node(); //Creates base root node that is returned
	tree->data.push_back("program"); //Pushes function name to node
	if (newToken.tokenInstance == "program") {
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i); //Gets new node for each subsequent function call
		tree->left = vars(fullInput, lineNumber, i); //Tree is created left to right
		tree->right = block(fullInput, lineNumber, i);
		return tree;
	} else {
		exitError(
				"Expected 'program' keyword, not " + newToken.tokenInstance
						+ " on line " + to_string(prevToken.lineNum + 1)); //Shows error and exits program
	}
}

Node* block(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("block");
	if (newToken.tokenInstance == "start") {
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		tree->left = vars(fullInput, lineNumber, i);
		tree->right = stats(fullInput, lineNumber, i);
		if (newToken.tokenInstance == "stop") {
			prevToken = newToken;
			newToken = getToken(fullInput, lineNumber, i);
			return tree;
		} else {
			exitError(
					"Expected keyword 'stop', not " + newToken.tokenInstance
							+ " on line " + to_string(prevToken.lineNum + 1));
		}
	} else {
		exitError(
				"Expected keyword 'start' or 'var', not "
						+ newToken.tokenInstance + " on line "
						+ to_string(prevToken.lineNum + 1));
	}
}

Node* vars(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("vars");
	if (newToken.tokenInstance == "var") {
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		tree->left = varList(fullInput, lineNumber, i);
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		return tree;
	} else {
		return tree;
	}
}

Node* varList(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("varList");
	if (getTokenString(tokenMap, newToken.id) == "IDENT_tk") {
		tree->data.push_back("ID");
		tree->data.push_back(newToken.tokenInstance);
		tree->data.push_back(to_string(newToken.lineNum));
		newToken = getToken(fullInput, lineNumber, i);
		if (newToken.tokenInstance == ",") {
			prevToken = newToken;
			newToken = getToken(fullInput, lineNumber, i);
			if (getTokenString(tokenMap, newToken.id) == "INT_tk") {
				tree->data.push_back("Integer");
				tree->data.push_back(newToken.tokenInstance);
				tree->data.push_back(to_string(newToken.lineNum));
				prevToken = newToken;
				newToken = getToken(fullInput, lineNumber, i);
				tree->left = x(fullInput, lineNumber, i);
				return tree;
			} else {
				exitError(
						"Expected an integer, not " + newToken.tokenInstance
								+ " on line "
								+ to_string(prevToken.lineNum + 1));
			}
		} else {
			exitError(
					"Expected a comma, not " + newToken.tokenInstance
							+ " on line " + to_string(prevToken.lineNum + 1));
		}
	} else {
		exitError(
				"Expected an identifier, not " + newToken.tokenInstance
						+ " on line " + to_string(prevToken.lineNum + 1));
	}
}

Node* x(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("x");
	if (newToken.tokenInstance == ";") {
		return tree;
	} else if (getTokenString(tokenMap, newToken.id) == "IDENT_tk") {
		tree->left = varList(fullInput, lineNumber, i);
		return tree;
	} else {
		exitError(
				"Expected an identifier or a ';', not " + newToken.tokenInstance
						+ " on line " + to_string(prevToken.lineNum + 1));
	}
}

Node* stats(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("stats");
	tree->left = stat(fullInput, lineNumber, i);
	tree->right = mStat(fullInput, lineNumber, i);
	return tree;
}

Node* stat(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("stat");
	if (newToken.tokenInstance == "read") {
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		tree->left = read(fullInput, lineNumber, i);
		return tree;
	} else if (newToken.tokenInstance == "print") {
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		tree->left = print(fullInput, lineNumber, i);
		return tree;
	} else if (newToken.tokenInstance == "start") {
		tree->left = block(fullInput, lineNumber, i);
		return tree;
	} else if (newToken.tokenInstance == "iff") {
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		tree->left = cond(fullInput, lineNumber, i);
		return tree;
	} else if (newToken.tokenInstance == "iterate") {
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		tree->left = iter(fullInput, lineNumber, i);
		return tree;
	} else if (newToken.tokenInstance == "set") {
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		tree->left = assign(fullInput, lineNumber, i);
		return tree;
	} else {
		exitError(
				"Statements must begin with 'read', 'print', 'start', 'iff', 'iterate', or 'set', not "
						+ newToken.tokenInstance + " on line "
						+ to_string(prevToken.lineNum + 1));
	}
}

Node* mStat(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("mStat");
	if (newToken.tokenInstance == "read" || newToken.tokenInstance == "print"
			|| newToken.tokenInstance == "start"
			|| newToken.tokenInstance == "iff"
			|| newToken.tokenInstance == "iterate"
			|| newToken.tokenInstance == "set") {
		tree->left = stat(fullInput, lineNumber, i);
		tree->right = mStat(fullInput, lineNumber, i);
		return tree;
	} else {
		return tree;
	}
}

Node* read(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("read");
	if (getTokenString(tokenMap, newToken.id) == "IDENT_tk") {
		tree->data.push_back("ID");
		tree->data.push_back(newToken.tokenInstance);
		tree->data.push_back(to_string(newToken.lineNum));
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		if (newToken.tokenInstance == ";") {
			newToken = getToken(fullInput, lineNumber, i);
			return tree;
		} else {
			exitError(
					"Expected ';', not " + newToken.tokenInstance + " on line "
							+ to_string(prevToken.lineNum + 1));
		}
	} else {
		exitError(
				"Expected an identifier, not " + newToken.tokenInstance
						+ " on line " + to_string(prevToken.lineNum + 1));
	}
}

Node* print(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("print");
	tree->left = exp(fullInput, lineNumber, i);
	if (newToken.tokenInstance == ";") {
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		return tree;
	} else {
		exitError(
				"Expecting ';', not " + newToken.tokenInstance + " on line "
						+ to_string(prevToken.lineNum));
	}
}

Node* cond(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("cond");
	if (newToken.tokenInstance == "[") {
		tree->data.push_back("Delimiter");
		tree->data.push_back("[");
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		tree->left = itCondL(fullInput, lineNumber, i); //This was split to keep tree binary, same with iter
		if (newToken.tokenInstance == "]") {
			tree->data.push_back("Delimiter");
			tree->data.push_back("]");
			prevToken = newToken;
			newToken = getToken(fullInput, lineNumber, i);
			tree->right = stat(fullInput, lineNumber, i);
			return tree;
		} else {
			exitError(
					"Expected a ']', not " + newToken.tokenInstance
							+ " on line " + to_string(prevToken.lineNum + 1));
		}
	} else {
		exitError(
				"Expected a '[', not " + newToken.tokenInstance + " on line "
						+ to_string(prevToken.lineNum + 1));
	}
}

Node* iter(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("iter");
	if (newToken.tokenInstance == "[") {
		tree->data.push_back("Delimiter");
		tree->data.push_back("[");
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		tree->left = itCondL(fullInput, lineNumber, i);
		if (newToken.tokenInstance == "]") {
			tree->data.push_back("Delimiter");
			tree->data.push_back("]");
			prevToken = newToken;
			newToken = getToken(fullInput, lineNumber, i);
			tree->right = stat(fullInput, lineNumber, i);
			return tree;
		} else {
			exitError(
					"Expected a ']', not " + newToken.tokenInstance
							+ " on line " + to_string(prevToken.lineNum + 1));
		}
	} else {
		exitError(
				"Expected a '[', not " + newToken.tokenInstance + " on line "
						+ to_string(prevToken.lineNum + 1));
	}
}

Node* itCondL(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("itCondL");
	tree->left = exp(fullInput, lineNumber, i);
	tree->right = itCondR(fullInput, lineNumber, i);
	return tree;
}

Node* itCondR(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("itCondR");
	tree->left = relational(fullInput, lineNumber, i);
	tree->right = exp(fullInput, lineNumber, i);
	return tree;
}

Node* assign(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("assign");
	if (getTokenString(tokenMap, newToken.id) == "IDENT_tk") {
		tree->data.push_back("ID");
		tree->data.push_back(newToken.tokenInstance);
		tree->data.push_back(to_string(newToken.lineNum));
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		tree->left = exp(fullInput, lineNumber, i);
		if (newToken.tokenInstance == ";") {
			prevToken = newToken;
			newToken = getToken(fullInput, lineNumber, i);
			return tree;
		} else {
			exitError(
					"Expected ';', not " + newToken.tokenInstance + " on line "
							+ to_string(prevToken.lineNum + 1));
		}
	} else {
		exitError(
				"Expected an identifier, not " + newToken.tokenInstance
						+ " on line " + to_string(prevToken.lineNum + 1));
	}
}

Node* relational(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("relational");
	if (newToken.tokenInstance == ".le." || newToken.tokenInstance == ".ge."
			|| newToken.tokenInstance == ".lt."
			|| newToken.tokenInstance == ".gt."
			|| newToken.tokenInstance == "**"
			|| newToken.tokenInstance == "~") {
		tree->data.push_back("Operator");
		tree->data.push_back(newToken.tokenInstance);
		tree->data.push_back(to_string(newToken.lineNum));
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		return tree;
	} else {
		exitError(
				"Expected a relational operator, not " + newToken.tokenInstance
						+ " on line " + to_string(prevToken.lineNum + 1));
	}
}

Node* exp(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("exp");
	tree->left = m(fullInput, lineNumber, i);
	if (newToken.tokenInstance == "+" || newToken.tokenInstance == "-") {
		tree->data.push_back("Operator");
		if (newToken.tokenInstance == "+")
			tree->data.push_back("+");
		else
			tree->data.push_back("-");
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		if (newToken.tokenInstance == "+") {
			exitError(
					"No double pluses allowed on line "
							+ to_string(prevToken.lineNum + 1));
		}
		tree->right = exp(fullInput, lineNumber, i);
		return tree;
	} else {
		return tree;
	}
}

Node* m(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("m");
	tree->left = n(fullInput, lineNumber, i);
	if (newToken.tokenInstance == "%") {
		tree->data.push_back("Operator");
		tree->data.push_back("%");
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		tree->right = m(fullInput, lineNumber, i);
		return tree;
	} else {
		return tree;
	}
}

Node* n(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("n");
	if (newToken.tokenInstance == "-") {
		tree->data.push_back("Operator");
		tree->data.push_back("-");
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		tree->left = n(fullInput, lineNumber, i); //Changed this from right to left
		return tree;
	}
	tree->left = r(fullInput, lineNumber, i);
	if (newToken.tokenInstance == "/") {
		tree->data.push_back("Operator");
		tree->data.push_back("/");
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		tree->right = n(fullInput, lineNumber, i);
		return tree;
	} else {
		return tree;
	}
}

Node* r(string fullInput, int *lineNumber, int *i) {
	Node *tree = new Node();
	tree->data.push_back("r");
	if (getTokenString(tokenMap, newToken.id) == "IDENT_tk"
			|| getTokenString(tokenMap, newToken.id) == "INT_tk") {
		if (getTokenString(tokenMap, newToken.id) == "IDENT_tk")
			tree->data.push_back("ID");
		else
			tree->data.push_back("Integer");
		tree->data.push_back(newToken.tokenInstance);
		tree->data.push_back(to_string(newToken.lineNum));
		prevToken = newToken;
		newToken = getToken(fullInput, lineNumber, i);
		return tree;
	} else if (newToken.tokenInstance == "(") {
		tree->data.push_back("Delimiter");
		tree->data.push_back("(");
		newToken = getToken(fullInput, lineNumber, i);
		tree->left = exp(fullInput, lineNumber, i);
		if (newToken.tokenInstance == ")") {
			tree->data.push_back("Delimiter");
			tree->data.push_back(")");
			prevToken = newToken;
			newToken = getToken(fullInput, lineNumber, i);
			return tree;
		} else {
			exitError(
					"Expected a ')', not " + newToken.tokenInstance
							+ " on line " + to_string(prevToken.lineNum + 1));
		}
	} else {
		exitError(
				"Expecting an identifier, an integer, or a '(', not "
						+ newToken.tokenInstance + " on line "
						+ to_string(prevToken.lineNum + 1));
	}
}
