#include <iostream>
#include <fstream>
#include <utility>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include "DatasetSentences.h"
#include "DatasetSentenceTrees.h"
#include "TestsetSentences.h"
#include "MatrixOperations.h"
#include "SentenceTree.h"
#include "Preprocessing.h"
#include "ValidationsetSentences.h"


#define EPSILON 0.000000000001
#define WORD_SIZE_VEC 30
#define THRESHOLD_ACCURACY 75
#define WINDOW_SIZE 10

#define BATCH_SIZE 27

using namespace std;

double learningRate = 0.01;
double regularizationParam = 0.0000001;
double regularizationParamWordRep = 0.00001;
double maxAcc = 0;
bool stop = false;

 ofstream rnnDebug("rnnDebug.txt");
 ofstream accuracyImprovement("accuracyImprovement.txt");
 ofstream outputResult("RNNClasificationResults.txt");
 ofstream plot("plot.cvs");

Vocabulary* vocab = new Vocabulary();
Dictionary* dictionary =  new Dictionary();
Vocabulary maxVocab = *vocab;
SentimentLabels* sentimentLabels =  new SentimentLabels();
DatasetSentences* datasetSentences = new DatasetSentences();
TestsetSentences* testsetSentences = new TestsetSentences();
DatasetSentencesTrees* targetSentencesTrees = new DatasetSentencesTrees();
ValidationsetSentences* devsetSentences = new ValidationsetSentences();

long long counterForAccuracy = 0;

unordered_map<string, long long> dataset_sentences = datasetSentences->getSentencesMap();
unordered_map<string, long long> testset_sentences = testsetSentences->getSentencesMap();
unordered_map<string, long long> devset_sentences = devsetSentences->getSentencesMap();

vector<double> zeros25 = getZeros(WORD_SIZE_VEC);

unordered_map<long long, string> treesParsedByMe = readParsedTrees();
vector<pair<string, long long>> vectorOfSentences = datasetSentences->getVectorOfSentences();

vector<double> predictionPerNGramLenght = getZeros(101);
vector<double> numberOfNGramSentences = getZeros(101);
long long numberOfNodesCorrectlyPredicted = 0;
long long totalNumberOfNodesCorrectlyPredicted = 0;

// Weights used to train the network. They have the dimension of 25 x 50.
vector<vector<double>> weights = initialiseWeigths(WORD_SIZE_VEC);
vector<vector<double>> maxWeights;
vector<double> biasWeights = zeros25;
vector<double> maxBiasWeights;

// Sentiment matrix used for classification in the 2 classes, negative and positive.
vector<vector<double>> sentimentMatrix;
vector<vector<double>> maxSentimentWeights;
vector<double> biasSentimentMatrix = getZeros(2);      // Bias of the sentiment matrix.
vector<double> maxSentimentBiasWeights;

vector<double> weightScore = createRandomDistributions(WORD_SIZE_VEC);


// Print the elements of a given matrix.
void printElementsOfMatrixDebug(vector<vector<double>> matrix) {
    for(int i = 0; i < matrix.size(); i++) {
        for(int j = 0; j < matrix[0].size(); j++) {
            rnnDebug<<" "<<matrix[i][j];
        }
        rnnDebug<<endl;
    }
    rnnDebug<<endl;
}

// Print the elements in a vector container.
void printElementsOfVectorDebug(vector<double> vec) {
    for(int i = 0; i < vec.size(); i++) {
    rnnDebug<<vec[i]<<" ; ";
    }
    rnnDebug<<endl;
}


unordered_map<long long, Tree*> getTargetTrees() {
    unordered_map<long long, Tree*> result;
    for (auto it = dataset_sentences.begin(); it != dataset_sentences.end(); it++) {
        string sentence = it->first;
        long long index = it->second;
        string targetTreeFormat = targetSentencesTrees->retrieveSentenceIndex(index);
        if (targetTreeFormat.empty()) {
             cout<<"For the given sentence, no target tree was found."<<endl;
             continue;
        }
        Tree* target = constructTargetTree(targetTreeFormat, sentence, dictionary, sentimentLabels);
        result.insert(make_pair(index, target));
    }
    return result;
}

unordered_map<long long, Tree*> targetParsedTrees = getTargetTrees();

void validateSentences() {
     long long counter = 0;
     long long totalNumberOfSentences = 0;
     string parsedTreesByMeRepresentation = "";
     string sentence;
     double error = 0;

     for (auto it = devset_sentences.begin(); it != devset_sentences.end(); it++  ) {
        totalNumberOfSentences++;
        if (counter % 100 == 0)cout<< " Classify Counter: "<<totalNumberOfSentences<<endl;
        sentence = it->first;
        long long index = it->second;
        long long indexInDictionary = dictionary->getPhraseIndex(sentence);
        parsedTreesByMeRepresentation = treesParsedByMe[index];
        string targetTreeFormat = targetSentencesTrees->retrieveSentenceIndex(index);

        Tree* parsedTree = useParserForCreatingTheTree(parsedTreesByMeRepresentation, sentence, vocab, sentimentMatrix, weights, biasSentimentMatrix, biasWeights);

        Tree* target = constructTargetTree(targetTreeFormat, sentence, dictionary, sentimentLabels);
        error -= computeErrorForTree(parsedTree, target, sentimentMatrix, biasSentimentMatrix);

        error += (double) (regularizationParam / 2) * getNormOfMatrix(sentimentMatrix);
        error += (double) (regularizationParam / 2) * getNormOfMatrix(weights);

        vector<double> finalScore = parsedTree->getScore();

        if (finalScore.size() == 2) {
            if (finalScore[0] > finalScore[1] ) {
                if (sentimentLabels->getSentimentScore(indexInDictionary) < 0.5) {
                       counter++;
                    //   outputResult<<finalScore[0]<<" "<<finalScore[1]<<" "<<sentimentLabels->getSentimentScore(indexInDictionary)<<" "<< "True Negative -> ";
                    } else outputResult<<finalScore[0]<<" "<<finalScore[1]<<" "<<sentimentLabels->getSentimentScore(indexInDictionary)<<" "<< "False Negative -> ";
                } else {
                 if (sentimentLabels->getSentimentScore(indexInDictionary) >= 0.5) {
                    counter++;
                   // outputResult<<finalScore[0]<<" "<<finalScore[1]<<" "<<sentimentLabels->getSentimentScore(indexInDictionary)<<" "<<"True Positive -> ";
                } else  outputResult<<finalScore[0]<<" "<<finalScore[1]<<" "<<sentimentLabels->getSentimentScore(indexInDictionary)<<" "<<"False Positive -> ";

                }
            outputResult<<sentence<<endl;
        } else {
            cout<<"Final score does not have 2 dimensions for the sentence:";
        }
        delete(parsedTree);
	delete(target);
     }
     cout<<"Correct predicted: "<<counter<<" out of: "<<totalNumberOfSentences;
     accuracyImprovement<<"Correct predicted while validating: "<<counter<<" out of: "<<totalNumberOfSentences <<endl<<endl;
     accuracyImprovement<<"Error while validating: "<<(double)error/totalNumberOfSentences<<endl<<endl;
     plot<<(double)(counter * 100)/totalNumberOfSentences<<" ";
     double accur = (double)(counter * 100)/totalNumberOfSentences;
      if (accur - maxAcc > EPSILON) {
                maxAcc = accur;
                maxWeights = weights;
                maxBiasWeights = biasWeights;
                maxSentimentWeights = sentimentMatrix;
                maxSentimentBiasWeights = biasSentimentMatrix;
        //      delete(maxVocab);
                maxVocab = *vocab;
        }
      if (maxAcc >= THRESHOLD_ACCURACY && accur < THRESHOLD_ACCURACY) {
        counterForAccuracy++;
     } else counterForAccuracy = 0;
     if (counterForAccuracy >=WINDOW_SIZE) stop = true;

}




void trainRNN() {
    int counter = 0;
    int correctPrediction = 0;
    vector<double> parentError;
    vector<double> nodeRep;
    double totalError;
    RNNParam* minibarchRNNParam = new RNNParam();

    vector<vector<double>> partialWeightsReg;
    vector<vector<double>> partialSentimentWeightsReg;
    vector<double> partialBiasSentimentMatrix;
    vector<double> partialBiasWeightsMatrix;

    std::random_shuffle ( vectorOfSentences.begin(), vectorOfSentences.end() );

    for (auto it = vectorOfSentences.begin(); it != vectorOfSentences.end(); it++) {
        counter++;
        if (counter % 100 == 0)
            cout<< " Training Counter: "<<counter<<endl;

        string sentence = it->first;
        long long index = it->second;
        string parsedTreesByMeRepresentation = treesParsedByMe[index];
        string targetTreeFormat = targetSentencesTrees->retrieveSentenceIndex(index);
        if (targetTreeFormat.empty()) {
             cout<<"For the given sentence, no target tree was found."<<endl;
             continue;
        }
      //  Tree* target = constructTargetTree(targetTreeFormat, sentence, dictionary, sentimentLabels);
        Tree* target = targetParsedTrees[index];

        Tree* parsedTree = useParserForCreatingTheTree(parsedTreesByMeRepresentation, sentence, vocab, sentimentMatrix, weights, biasSentimentMatrix, biasWeights);

        if (parsedTree->getScore()[0] > parsedTree->getScore()[1] && target->getRootRepresentation()[0] == 1)
                correctPrediction++;
        if (parsedTree->getScore()[1] > parsedTree->getScore()[0] && target->getRootRepresentation()[1] == 1)
                correctPrediction++;

        nodeRep = parsedTree->getRootRepresentation();
  //      printElementsOfVectorDebug(nodeRep);

        RNNParam* result = backprop(target, parsedTree, sentimentMatrix, weights, zeros25, biasSentimentMatrix);

        vector<double> score = parsedTree->getScore();
        vector<double> targetScore = target->getRootRepresentation();

        minibarchRNNParam->updateWeightsMatrix(result->getWeightsMatrix());

        minibarchRNNParam->updateSentimentWeightsMatrix(result->getSentimentWeightsMatrix());

        minibarchRNNParam->updateVocabError(result->getVocabError());

        minibarchRNNParam->updateBiasSentimentMatrix(result->getBiasSentimentMatrix());

        minibarchRNNParam->updateBiasWeightMatrix(result->getBiasWeightMatrix());

        minibarchRNNParam->updateTotalError(result->getTotalError());

        if (counter % BATCH_SIZE == 0 || counter == dataset_sentences.size()) {
            double batchSize = BATCH_SIZE;

            if (counter == dataset_sentences.size() && (counter % BATCH_SIZE != 0)) {
                int temp = counter / BATCH_SIZE;
                int newBatchSize = counter - temp * BATCH_SIZE;
                batchSize = newBatchSize;
            }
            double scale = (double) 1 / batchSize;
           // cout<<scale<<" "<<4.56 * 2<<endl;
            double minibatchError = 0 - scale * minibarchRNNParam->getTotalError();
            minibatchError += (double) (regularizationParam / 2) * getNormOfMatrix(sentimentMatrix);
            minibatchError += (double) (regularizationParam / 2) * getNormOfMatrix(weights);



            totalError += minibatchError * ((double) 1/batchSize);

            partialWeightsReg = multiplyMatrixByScalar(weights, regularizationParam);
            partialWeightsReg = addTwoMatrices(minibarchRNNParam->getWeightsMatrix(), partialWeightsReg);
            partialWeightsReg = multiplyMatrixByScalar(partialWeightsReg, scale);
            weights = subtractTwoMatrices(weights, multiplyMatrixByScalar(partialWeightsReg, learningRate));

            partialSentimentWeightsReg = multiplyMatrixByScalar(sentimentMatrix, regularizationParam);
            partialSentimentWeightsReg = addTwoMatrices(minibarchRNNParam->getSentimentWeightsMatrix(), partialSentimentWeightsReg);
            partialSentimentWeightsReg = multiplyMatrixByScalar(partialSentimentWeightsReg, scale);
            sentimentMatrix = subtractTwoMatrices(sentimentMatrix, multiplyMatrixByScalar(partialSentimentWeightsReg, learningRate));

            partialBiasSentimentMatrix = minibarchRNNParam->getBiasSentimentMatrix();
            partialBiasSentimentMatrix = multiplyVectorByScalar(partialBiasSentimentMatrix, scale);
            biasSentimentMatrix = substractTwoVectors(biasSentimentMatrix, multiplyVectorByScalar(partialBiasSentimentMatrix, learningRate));

            partialBiasWeightsMatrix = minibarchRNNParam->getBiasWeightMatrix();
            partialBiasWeightsMatrix = multiplyVectorByScalar(partialBiasWeightsMatrix, scale);
            biasWeights = substractTwoVectors(biasWeights, multiplyVectorByScalar(partialBiasWeightsMatrix, learningRate));

            unordered_map<string, vector<double>> wordUpdates = minibarchRNNParam->getVocabError();
            for(auto j = wordUpdates.begin(); j != wordUpdates.end(); ++j) {
                vector<double> errorForWord = j->second;
                errorForWord = multiplyVectorByScalar(errorForWord, scale);
                vocab->updateWordRepresentation(j->first, j->second, learningRate);
            }
            minibarchRNNParam->resetFields();
        }

     //delete(target);
     delete(parsedTree);
     delete(result);
    }
    accuracyImprovement<<"Correct predicted while training: "<<correctPrediction<<" out of: "<<counter <<endl<<endl;
    accuracyImprovement<<"Error while training: "<<(double)totalError<<endl<<endl;
    plot<<(double)(correctPrediction * 100)/counter<<" ";
    delete(minibarchRNNParam);
}

void classifyTestSentences() {
     long long counter = 0;
     long long totalNumberOfSentences = 0;
     string parsedTreesByMeRepresentation = "";
     string sentence;

     for (auto it = testset_sentences.begin(); it != testset_sentences.end(); it++) {
        totalNumberOfSentences++;
        if (counter % 100 == 0)cout<< " Classify Counter: "<<totalNumberOfSentences<<endl;
        sentence = it->first;
        long long index = it->second;
        long long indexInDictionary = dictionary->getPhraseIndex(sentence);
        parsedTreesByMeRepresentation = treesParsedByMe[index];
        string targetTreeFormat = targetSentencesTrees->retrieveSentenceIndex(index);
        int numberOfWordsInSentence = getWordsFromSentence(sentence).size();

        Tree* target = constructTargetTree(targetTreeFormat, sentence, dictionary, sentimentLabels);
        Tree* parsedTree = useParserForCreatingTheTree(parsedTreesByMeRepresentation, sentence, vocab, sentimentMatrix, weights, biasSentimentMatrix, biasWeights);

        long long val = getTotalNumberOfInnerNodesCorrectlyPredictted(target, parsedTree);
        numberOfNodesCorrectlyPredicted += val;
        totalNumberOfNodesCorrectlyPredicted += numberOfWordsInSentence + numberOfWordsInSentence - 1;

        vector<double> finalScore = parsedTree->getScore();
        numberOfNGramSentences[numberOfWordsInSentence]++;

        if (finalScore.size() == 2) {
            if (finalScore[0] > finalScore[1] ) {
                if (sentimentLabels->getSentimentScore(indexInDictionary) < 0.5) {
                       counter++;
                       predictionPerNGramLenght[numberOfWordsInSentence]++;
                       outputResult<<finalScore[0]<<" "<<finalScore[1]<<" "<<sentimentLabels->getSentimentScore(indexInDictionary)<<" "<< "True Negative -> ";
                    } else outputResult<<finalScore[0]<<" "<<finalScore[1]<<" "<<sentimentLabels->getSentimentScore(indexInDictionary)<<" "<< "False Negative -> ";
                } else {
                 if (sentimentLabels->getSentimentScore(indexInDictionary) >= 0.5) {
                    counter++;
                    predictionPerNGramLenght[numberOfWordsInSentence]++;
                    outputResult<<finalScore[0]<<" "<<finalScore[1]<<" "<<sentimentLabels->getSentimentScore(indexInDictionary)<<" "<<"True Positive -> ";
                } else  outputResult<<finalScore[0]<<" "<<finalScore[1]<<" "<<sentimentLabels->getSentimentScore(indexInDictionary)<<" "<<"False Positive -> ";

                }
            outputResult<<sentence<<endl;
        } else {
            cout<<"Final score does not have 2 dimensions for the sentence:";
        }
        //outputResult<<"Sentence: "<<sentence<<endl;
        delete(parsedTree);
        delete(target);
     }
     cout<<"Correct predicted: "<<counter<<" out of: "<<totalNumberOfSentences;
     accuracyImprovement<<"Correct predicted while classifying: "<<counter<<" out of: "<<totalNumberOfSentences <<endl<<endl;
     plot<<(double)(counter* 100)/totalNumberOfSentences<<endl;
}

int main()
{
    std::srand ( unsigned ( std::time(0) ) );
    preprocessSentences();
    vector<double> weights1 = createRandomDistributions(WORD_SIZE_VEC);
    sentimentMatrix.push_back(weights1);
    weights1 = createRandomDistributions(WORD_SIZE_VEC);
    sentimentMatrix.push_back(weights1);

     int scaleLearningRate = 30;
    for (int i = 0; i < 200; i++) {
        accuracyImprovement<<"Iteration: "<<i<<endl;
            if (learningRate > 0.00001 && i % scaleLearningRate == 0) {
                learningRate = learningRate / 2;
               // scaleLearningRate--;
        }
        plot<<i<<" ";
        trainRNN();
        validateSentences();
         if (stop == true) {
                cout<<"I am stopping after "<<i<<" iterations"<<endl;
                maxWeights = weights;
                maxBiasWeights = biasWeights;
                maxSentimentWeights = sentimentMatrix;
                maxSentimentBiasWeights = biasSentimentMatrix;
                vocab = &maxVocab;
                classifyTestSentences();
                plot<<endl;
                for(int i = 0; i < numberOfNGramSentences.size(); i++) {
                    plot<<"("<<predictionPerNGramLenght[i]<< ", "<<numberOfNGramSentences[i]<<") ";
                }

                break;
        } else plot<<endl;

        printElementsOfMatrix(sentimentMatrix);
        cout<<"*************** Biased Sent"<<endl;
        printElementsOfVector(biasSentimentMatrix);
        cout<<"*************** Biased Weights"<<endl;
        printElementsOfVector(biasWeights);
        cout<<"*************** Sentiment Matrix"<<endl;
    }


    cout << "Hello world!" << endl;
    return 0;
}
