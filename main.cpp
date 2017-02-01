#include <iostream>
#include <fstream>
#include <utility>
#include <iomanip>
#include "DatasetSentences.h"
#include "DatasetSentenceTrees.h"
#include "TestsetSentences.h"
#include "MatrixOperations.h"
#include "SentenceTree.h"
#include "Preprocessing.h"

#define EPSILON 0.000000000001


using namespace std;

//double learningRate = 0.001;
//double learningRate = 0.0001;
double learningRate = 0.0007;
double regularizationParam = 0.25;

 ofstream rnnDebug("rnnDebug.txt");
 ofstream accuracyImprovement("accuracyImprovement.txt");
 ofstream outputResult("RNNClasificationResults.txt");

Vocabulary* vocab = new Vocabulary();
Dictionary* dictionary =  new Dictionary();
SentimentLabels* sentimentLabels =  new SentimentLabels();
DatasetSentences* datasetSentences = new DatasetSentences();
TestsetSentences* testsetSentences = new TestsetSentences();
DatasetSentencesTrees* targetSentencesTrees = new DatasetSentencesTrees();
RNNParam* minibarchRNNParam = new RNNParam();

unordered_map<string, long long> dataset_sentences = datasetSentences->getSentencesMap();
unordered_map<string, long long> testset_sentences = testsetSentences->getSentencesMap();

// Weights used to train the network. They have the dimension of 25 x 50.
vector<vector<double>> weights = initialiseWeigths(25);

// Sentiment matrix used for classification in the 2 classes, negative and positive.
vector<vector<double>> sentimentMatrix;

vector<double> weightScore = createRandomDistributions(25);


// Print the elements of a given matrix.
void printElementsOfMatrixDebug(vector<vector<double>> matrix) {
    for(int i = 0; i < matrix.size(); i++) {
        for(int j = 0; j < matrix[0].size(); j++) {
            rnnDebug<<" "<<matrix[i][j];
        }
        rnnDebug<<endl;
    }
}

// Print the elements in a vector container.
void printElementsOfVectorDebug(vector<double> vec) {
    for(int i = 0; i < vec.size(); i++) {
    rnnDebug<<vec[i]<<" ; ";
    }
    rnnDebug<<endl;
}



void trainRNN() {
    int counter = 0;
    int correctPrediction = 0;
    vector<double> parentError;
    vector<double> nodeRep;
    vector<vector<double>> partialWeightsReg;
    vector<vector<double>> partialSentimentWeightsReg;
    unordered_map<long long, string> treesParsedByMe = readParsedTrees();
    for (auto it = dataset_sentences.begin(); it != dataset_sentences.end(); ++it) {
        counter++;
        if (counter % 100 == 0)
            cout<< " Training Counter: "<<counter<<endl;

        rnnDebug<<endl<<endl<<"Exemple: "<<counter<<endl<<endl;
        string sentence = it->first;
        long long index = it->second;
        string parsedTreesByMeRepresentation = treesParsedByMe[index];
        string targetTreeFormat = targetSentencesTrees->retrieveSentenceIndex(index);
        if (targetTreeFormat.empty()) {
             cout<<"For the given sentence, no target tree was found."<<endl;
             continue;
        }
        Tree* target = constructTargetTree(targetTreeFormat, sentence, dictionary, sentimentLabels);
        Tree* parsedTree = useParserForCreatingTheTree(parsedTreesByMeRepresentation, sentence, dictionary, vocab, sentimentMatrix, weights);
        if (parsedTree->getScore()[0] > parsedTree->getScore()[1] && target->getRootRepresentation()[0] == 1)
                correctPrediction++;
        if (parsedTree->getScore()[1] > parsedTree->getScore()[0] && target->getRootRepresentation()[1] == 1)
                correctPrediction++;
       // accuracyImprovement<< "Parsed tree score"
       // parsedTree->inOrderTraversal();
        parentError = getZeros(25);
        rnnDebug<<"*************"<<endl;
        nodeRep = parsedTree->getRootRepresentation();
        printElementsOfVectorDebug(nodeRep);
        RNNParam* result = backprop(target,parsedTree, sentimentMatrix, weights,parentError, vocab);
        vector<double> score = parsedTree->getScore();
        vector<double> targetScore = target->getRootRepresentation();

        //vector<vector<double>> resultSentimentMatrix = computeSentimentMatrixPartialDerivative(target, parsedTree);

        partialWeightsReg = multiplyMatrixByScalar(weights, regularizationParam);
        partialWeightsReg = addTwoMatrices(result->getWeightsMatrix(), partialWeightsReg);
      //  weights = subtractTwoMatrices(weights, multiplyMatrixByScalar(result->getWeightsMatrix(), learningRate));
        weights = subtractTwoMatrices(weights, multiplyMatrixByScalar(partialWeightsReg, learningRate));

        partialSentimentWeightsReg = multiplyMatrixByScalar(sentimentMatrix, regularizationParam / 10);
        partialSentimentWeightsReg = addTwoMatrices(result->getSentimentWeightsMatrix(), partialSentimentWeightsReg);
        //sentimentMatrix = subtractTwoMatrices(sentimentMatrix, multiplyMatrixByScalar(result->getSentimentWeightsMatrix(), learningRate));
        sentimentMatrix = subtractTwoMatrices(sentimentMatrix, multiplyMatrixByScalar(partialSentimentWeightsReg, learningRate));

        unordered_map<string, vector<double>> wordUpdates = result->getVocabError();
        for(auto j = wordUpdates.begin(); j != wordUpdates.end(); ++j) {
            //vocab->updateWordRepresentation(j->first, j->second, learningRate);
            vocab->updateWordRepresentation(j->first, j->second, learningRate, regularizationParam / 10);
        }

        /*minibarchRNNParam->updateSentimentWeightsMatrix(result->getSentimentWeightsMatrix());
        minibarchRNNParam->updateWeightsMatrix(result->getWeightsMatrix());
        unordered_map<string, vector<double>> wordUpdates = result->getVocabError();
        for(auto j = wordUpdates.begin(); j != wordUpdates.end(); ++j) {
            minibarchRNNParam->updateVocabError(j->first, j->second);
        }
         if (counter % 1 == 0 || counter == dataset_sentences.size()) {
            double minibatchLearningRate = (1/1) * learningRate;
            weights = subtractTwoMatrices(weights, multiplyMatrixByScalar(minibarchRNNParam->getWeightsMatrix(), minibatchLearningRate));
            sentimentMatrix = subtractTwoMatrices(minibarchRNNParam->getSentimentWeightsMatrix(), multiplyMatrixByScalar(minibarchRNNParam->getSentimentWeightsMatrix(), minibatchLearningRate));
            unordered_map<string, vector<double>> wordUpdates = minibarchRNNParam->getVocabError();
            for(auto j = wordUpdates.begin(); j != wordUpdates.end(); ++j) {
                vocab->updateWordRepresentation(j->first, j->second, minibatchLearningRate);
            }
            minibarchRNNParam = new RNNParam();
        }*/


       /* rnnDebug<<"Weight Matrix error "<<endl<<endl;
        printElementsOfMatrixDebug(result->getWeightsMatrix());
        rnnDebug<<"Weight Matrix updated "<<endl<<endl;
        printElementsOfMatrixDebug(weights);
        rnnDebug<<endl<<"Score Weight Matrix error"<<endl<<endl;
        printElementsOfMatrixDebug(result->getSentimentWeightsMatrix());
        rnnDebug<<endl<<"Score Weight Matrix error"<<endl<<endl;
        printElementsOfMatrixDebug(sentimentMatrix);
        rnnDebug<<endl; */

     delete(target);
     delete(parsedTree);
     delete(result);
    }
    accuracyImprovement<<"Correct predicted while training: "<<correctPrediction<<" out of: "<<counter <<endl<<endl;

}

void classifyTestSentences() {
     long long counter = 0;
     long long totalNumberOfSentences = 0;
     string parsedTreesByMeRepresentation = "";
     string sentence;
     unordered_map<long long, string> treesParsedByMe = readParsedTrees();
     for (auto it = testset_sentences.begin(); it != testset_sentences.end(); ++it ) {
        totalNumberOfSentences++;
        if (counter % 100 == 0)cout<< " Classify Counter: "<<totalNumberOfSentences<<endl;
        sentence = it->first;
        long long index = it->second;
        long long indexInDictionary = dictionary->getPhraseIndex(sentence);
        parsedTreesByMeRepresentation = treesParsedByMe[index];
        //cout<<parsedTreesByMeRepresentation<<endl;
        //Tree* parsedTree = constructTreeForASentence(sentence, weights, sentimentMatrix, vocab);
        Tree* parsedTree = useParserForCreatingTheTree(parsedTreesByMeRepresentation, sentence, dictionary, vocab, sentimentMatrix, weights);
        vector<double> finalScore = parsedTree->getScore();
       // printElementsOfVector(parsedTree->getScore());
        if (finalScore.size() == 2) {
            if (finalScore[0] > finalScore[1] ) {
                if (sentimentLabels->getSentimentScore(indexInDictionary) < 0.5) {
                       counter++;
                       outputResult<<finalScore[0]<<" "<<finalScore[1]<<" "<<sentimentLabels->getSentimentScore(indexInDictionary)<<" "<< "True Negative -> ";
                    } else outputResult<<finalScore[0]<<" "<<finalScore[1]<<" "<<sentimentLabels->getSentimentScore(indexInDictionary)<<" "<< "False Negative -> ";
                } else {
                 if (sentimentLabels->getSentimentScore(indexInDictionary) >= 0.5) {
                    counter++;
                    outputResult<<finalScore[0]<<" "<<finalScore[1]<<" "<<sentimentLabels->getSentimentScore(indexInDictionary)<<" "<<"True Positive -> ";
                } else  outputResult<<finalScore[0]<<" "<<finalScore[1]<<" "<<sentimentLabels->getSentimentScore(indexInDictionary)<<" "<<"False Positive -> ";

                }
            outputResult<<sentence<<endl;
        } else {
            cout<<"Final score does not have 2 dimensions for the sentence:";
        }
        //outputResult<<"Sentence: "<<sentence<<endl;
        delete(parsedTree);
     }
     cout<<"Correct predicted: "<<counter<<" out of: "<<totalNumberOfSentences;
     accuracyImprovement<<"Correct predicted while classifying: "<<counter<<" out of: "<<totalNumberOfSentences <<endl<<endl;
}

int main()
{
    preprocessSentences();
    vector<double> weights1 = createRandomDistributions(25);
    sentimentMatrix.push_back(weights1);
    weights1 = createRandomDistributions(25);
    sentimentMatrix.push_back(weights1);
    weights1 = createRandomDistributions(25);

    // printElementsOfMatrixDebug(matrxMult2);
   /* cout<<"+++"<<endl;
    Tree* parsedTree2 = constructTreeForASentence("But he somehow pulls it off .", weights, sentimentMatrix, vocab);
    string treeText = "9|8|6|6|7|7|8|9|0";
    string sente = "Um , no . .";

    Tree* parsedTree = useParserForCreatingTheTree(treeText, sente, dictionary, vocab, sentimentMatrix, weights);

    vector<double> parentError;

    for(int i = 0; i < 25;i++) parentError.push_back(0.0);*/

    for (int i = 0; i < 50; i++) {
        accuracyImprovement<<"Iteration: "<<i<<endl;
        trainRNN();
        classifyTestSentences();
    }
   // parsedTree->inOrderTraversal();
    cout << "Hello world!" << endl;
    return 0;
}
