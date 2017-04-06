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
Tree* constructTreeForASentence(string sentence, vector<vector<double>> weights, vector<vector<double>> weightScore, Vocabulary *vocab, vector<double> biasSentimentMatrix);


// Given a string of digits and | symbols, return the target tree represented by that string.
Tree* constructTargetTree(string treeText, string sentence, Dictionary* dictionary, SentimentLabels* sentimentLabels);


RNNParam* backprop(Tree * targetTree, Tree * computedTree, vector<vector<double>>
            weightScoresMatrix, vector<vector<double>> weightsMatrix, vector<double> parentError, vector<double> biasSentimentMatrix);


// Read from the PreprocessedDatasetSentences.txt.
unordered_map<long long, string> readParsedTrees();

// Use the parsing tring in the forward propogation.
Tree* useParserForCreatingTheTree(string treeText, string sentence, Vocabulary* vocab, vector<vector<double>> weightScoresMatrix,
        vector<vector<double>> weightsMatrix, vector<double> biasSentimentMatrix, vector<double> biasWeights);

double computeErrorForTree(Tree* computedTree, Tree* targetTree, vector<vector<double>> weightScoresMatrix, vector<double> biasWeights);

long long getTotalNumberOfInnerNodesCorrectlyPredictted(Tree* targetTree, Tree* computedTree);
