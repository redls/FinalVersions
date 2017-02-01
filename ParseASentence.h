#include<cstdio>
#include<vector>
#include<string>
#include "Tree.h"
#include "Vocabulary.h"

using namespace std;

/**
 * Methods dealing with sentence operations like.
 */

// Retrieve the words from the senetence and store them into an array.
vector<string> getWordsFromSentence(string sentence);

// Retrieve the word representation from the Vocabulary and add them into a vector.
vector<Node> retrieveWordRepresentation(vector<string> words, Vocabulary *vocabulary);

// Create random distributions of size d.
vector<double> createRandomDistributions(int d);

/**
 * Initialise the weights of the neural network. This will return and array containing d distribuitions, each
 * of 2*d elements.
 */
vector<vector<double>> initialiseWeigths(int dimension);
