#include "Node.h"
#include<cstdio>
#include<vector>

using namespace std;

/**
 * Implementation of the methods defined for a node which is used in the Recursive Neural Network.
 */

Node::Node() {
}

Node::~Node() {
}

Node::Node(vector<double> newWord) {
    for(int i = 0; i<newWord.size();i++) {
        wordRepresentation.push_back(newWord[i]);
    }
}

vector<double> Node::getNode() {
    return wordRepresentation;
}


