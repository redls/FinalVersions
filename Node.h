#include<cstdio>
#include<vector>

using namespace std;

/**
 * Class which represent a node in the parsing tree of the Recursive Neural Network.
 */

class Node {
   vector<double> wordRepresentation;

   public:
    Node();
    ~Node();
    Node(vector<double> newWord);
    vector<double> getNode();
};
