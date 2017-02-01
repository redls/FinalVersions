#include "Node.h"
#include<cstdio>
#include<vector>
#include<string>

using namespace std;

class Tree {
    Node node;
    Tree *left;
    Tree *right;
    vector<double> score;
    string word = "";

    public:
        Tree(Node x);
        ~Tree();
        void addNewNodeToTree(Node x);
        vector<double> getRootRepresentation();
        void setLeftTree(Tree *x);
        void setRightTree(Tree *x);
        void setRoot(Node x);
        void setRoot(vector<double> t);
        void setScore(vector<double> s);
        void setWord(string word);
        string getWord();
        Tree* getLeftTree();
        Tree* getRightTree();
        vector<double> getScore();

        // Print the elements of the tree in in-order.
        void inOrderTraversal();
};
