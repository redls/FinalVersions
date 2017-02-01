#include "Tree.h"
#include<cstdio>
#include<vector>
#include<iostream>

using namespace std;

Tree::Tree(Node x) {
    left = nullptr;
    right = nullptr;
    node = Node(x.getNode());
    word = "";
}

Tree::~Tree() {
    delete(left);
    delete(right);
}

void Tree::addNewNodeToTree(Node x) {

}

vector<double> Tree::getRootRepresentation() {
    vector<double> rootRepresentation = this->node.getNode();
    return rootRepresentation;
}

void Tree::setLeftTree(Tree* x) {
    this->left = x;
}

void Tree::setRightTree(Tree* x) {
    this->right = x;
}

void Tree::setRoot(Node x) {
    this->node = x;
}

void Tree::setRoot(vector<double> x) {
    Node* n = new Node(x);
    this->node = *n;
}

void Tree::setScore(vector<double> s) {
    this->score = s;
}

void Tree::setWord(string x) {
    this->word = x;
}

Tree* Tree::getLeftTree() {
    return this->left;
}

Tree* Tree::getRightTree() {
    return this->right;
}

vector<double> Tree::getScore() {
    return this->score;
}

string Tree::getWord() {
    return this->word;
}

// Recursive method for printing a tree.
void printTree(Tree* t) {
    if (t == nullptr) return;
    cout<<"(";
    printTree(t->getLeftTree());
    vector<double> nodeRep = t->getRootRepresentation();
    cout<<"Root ";
    for (int i = 0; i < nodeRep.size();i++) {
        cout<<nodeRep[i]<<" ";
    }
    cout<<endl;
    printTree(t->getRightTree());
     cout<<")";
}


// Print the elements of the tree in-order.
void Tree::inOrderTraversal() {
    Tree* t = this;
    printTree(t);
    cout<<endl;
}
