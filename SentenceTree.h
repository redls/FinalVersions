#include "ParseASentence.h"
#include "Dictionary.h"
#include "SentimentLabels.h"
#include "RNNParam.h"
#include<string>
#include<unordered_map>
using namespace std;

/**
 * Contains methods which interact with a given sentence's tree.
 */


// Given a sentence, construct its semantic tree by combining the words using the tanh rule.
Tree* constructTreeForASentence(string sentence, vector<vector<double>> weights, vector<vector<double>> weightScore, Vocabulary *vocab);


// Given a string of digits and | symbols, return the target tree represented by that string.
Tree* constructTargetTree(string treeText, string sentence, Dictionary* dictionary, SentimentLabels* sentimentLabels);


RNNParam* backprop(Tree * targetTree, Tree * computedTree, vector<vector<double>>
            weightScoresMatrix, vector<vector<double>> weightsMatrix, vector<double> parentError, Vocabulary* vocab);


// Read from the PreprocessedDatasetSentences.txt.
unordered_map<long long, string> readParsedTrees();

// Use the parsing tring in the forward propogation.
Tree* useParserForCreatingTheTree(string treeText, string sentence, Dictionary* dictionary, Vocabulary* vocab,
    vector<vector<double>> weightScoresMatrix, vector<vector<double>> weightsMatrix);

// Compute Sentiment Matrix partial derivative.
//vector<vector<double>> computeSentimentMatrixPartialDerivative(Tree* targetTree, Tree* computedTree);
