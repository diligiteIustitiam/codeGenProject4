#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <map>
#include "node.h"
#include "parser.h"
#include "staticSemantics.h"
#include "codeGeneration.h"
using namespace std;

/*static void inorder(Node *root, string space) {
	if (root == nullptr) //Replace sets with vectors! they alphabetize
		return;

	inorder(root->left, space + "  ");
	cout << space;
	for (const string &x : root->data) {
		cout << x + " ";
	}
	cout << endl;
	inorder(root->right, space + "  ");
}

static void preorder(Node *root, string space) {
	if (root == nullptr) //Replace sets with vectors! they alphabetize
		return;

	cout << space;
	for (const string &x : root->data) {
		cout << x + " ";
	}
	cout << endl;
	preorder(root->left, space + "  ");
	preorder(root->right, space + "  ");
}*/

static void deleteTree(Node *root) {
	if (root == nullptr) {
		return;
	}
	deleteTree(root->left);
	deleteTree(root->right);
	delete root;
}

int main(int argc, char *argv[]) {
	string fileName;
	string outputName;
	BinaryTree tree;
	string space = "";
	if (argc > 2)
		exitError("Too many inputs, only enter one input!");
	if (argc == 2) {
		fileName = argv[1];
		outputName = fileName;
		string fullFileName = fileName + ".ext";
		ifstream file;
		file.open(fullFileName);
		if (!file)
			exitError(
					"This file does not exist, or does not end with extension '.ext'!");
		tree.root = parser(fullFileName);
		checkSemantics(tree.root);
		checkUnused();
	}
	if (argc == 1) {
		tree.root = parser();
		checkSemantics(tree.root);
		checkUnused();
		outputName = "a";
	}
	//preorder(tree.root, space); //preorder shows what tree really looks like, inorder shows what we go through
	ofstream MyFile(outputName + ".asm");
	for (size_t x = 0; x < STV.variables.size(); x++)
		globalVars += STV.variables[x] + " 0\n";
	recGen(tree.root, MyFile);
	MyFile << endl << "STOP" << endl;
	MyFile << globalVars;
	MyFile.close();
	cout << "Done and done!" << endl;
	deleteTree(tree.root);
	return 0;
}
