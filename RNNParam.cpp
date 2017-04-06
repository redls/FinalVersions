#include<cstdio>
#include<string>
#include<unordered_map>
#include<iostream>
#include "RNNParam.h"
#include "MatrixOperations.h"

using namespace std;

#define NUMBER_OF_CLASSES 2
#define WORD_SIZE_VEC 30

RNNParam::RNNParam() {
    sentimentWeightsMatrix.clear();
    biasSentimentMatrix = getZeros(NUMBER_OF_CLASSES);

    weightsMatrix.clear();
    vector<double> aux = getZeros(2 * WORD_SIZE_VEC);
    for(int i = 0; i < WORD_SIZE_VEC; i++) weightsMatrix.push_back(aux);

    vector<double> aux2 = getZeros(WORD_SIZE_VEC);
    sentimentWeightsMatrix.push_back(aux2);
    sentimentWeightsMatrix.push_back(aux2);

    biasWeightMatrix = aux2;
    vocabError.clear();
    totalError = 0;
}


RNNParam::RNNParam(int dimesion) {
    sentimentWeightsMatrix.clear();
    weightsMatrix.clear();
    biasSentimentMatrix = getZeros(NUMBER_OF_CLASSES);
    // Set up weights matrix.
    vector<double> aux = getZeros(dimesion * 2);
    for(int i = 0; i < dimesion; i++) weightsMatrix.push_back(aux);

    // Set up score matrix.
    vector<double> aux2 = getZeros(dimesion);
    for(int i = 0; i < dimesion; i++) {
        sentimentWeightsMatrix.push_back(aux2);
    }

    biasWeightMatrix = aux2;

    vocabError.clear();
    totalError = 0;

}

void RNNParam::resetFields() {
    sentimentWeightsMatrix.clear();
    weightsMatrix.clear();
    biasSentimentMatrix = getZeros(NUMBER_OF_CLASSES);
    vector<double> aux = getZeros(2 * WORD_SIZE_VEC);
    for(int i = 0; i < WORD_SIZE_VEC; i++) weightsMatrix.push_back(aux);
    vector<double> aux2 = getZeros(WORD_SIZE_VEC);
    sentimentWeightsMatrix.push_back(aux2);
    sentimentWeightsMatrix.push_back(aux2);

    biasWeightMatrix = aux2;

    vocabError.clear();
    totalError = 0;
}

void RNNParam::resetVocab() {
    vocabError.clear();
}

void RNNParam::setSentimentWeightsMatrix(vector<vector<double>> x) {
    this->sentimentWeightsMatrix = x;
}

void RNNParam::setBiasSentimentMatrix(vector<double> x) {
    this->biasSentimentMatrix = x;
}

void RNNParam::setBiasWeightMatrix(vector<double> x) {
    this->biasWeightMatrix = x;
}

void RNNParam::updateBiasSentimentMatrix(vector<double> x) {
    if (x.size() != biasSentimentMatrix.size()) {
        cout<<"Couldn't update the biasSentimentMatrix because the given input has different dimension."<<endl;
    }

    for(int i = 0; i < biasSentimentMatrix.size(); i++) {
        biasSentimentMatrix[i] += x[i];
    }
}

void RNNParam::updateBiasWeightMatrix(vector<double> x) {
    if (x.size() != biasWeightMatrix.size()) {
        cout<<"Couldn't update the updateBiasWeightMatrix because the given input has different dimension."<<endl;
    }

    for(int i = 0; i < biasWeightMatrix.size(); i++) {
        biasWeightMatrix[i] += x[i];
    }
}

void RNNParam::setTotalError(double x) {
    this->totalError = x;
}

void RNNParam::updateSentimentWeightsMatrix(vector<vector<double>> x) {
    if (x.size() != sentimentWeightsMatrix.size() || x[0].size() != sentimentWeightsMatrix[0].size()) {
        cout<<"Sentiment matrix couldn't be updated due to different dimensions. (updateSentimentWeightsMatrix())."<<endl;
  //      exit(0);
    }
    for(int i = 0; i < x.size(); i++) {
        for(int j = 0; j < x[0].size(); j++) {
            sentimentWeightsMatrix[i][j] += x[i][j];
        }
    }
}

void RNNParam::setWeightsMatrix(vector<vector<double>> x) {
    this->weightsMatrix = x;
}

void RNNParam::updateWeightsMatrix(vector<vector<double>> x) {
    if (x.size() != weightsMatrix.size() ||x[0].size() != weightsMatrix[0].size()) {
        cout<<"Sentiment matrix couldn't be updated due to different dimensions. (updateWeightsMatrix())."<<endl;
       // exit(0);
    }
    for(int i = 0; i < x.size(); i++) {
        for(int j = 0; j < x[0].size(); j++) {
            this->weightsMatrix[i][j] += x[i][j];
        }
    }
}

RNNParam::~RNNParam(){
}

// Search for the word in the map. If an entry has already been created,
// add the error to this word.
void RNNParam::updateVocabError(string word, vector<double> error) {
    unordered_map<string, vector<double>>::const_iterator found_iter = vocabError.find(word);
    if (found_iter == vocabError.end()) {
        vocabError.insert(make_pair(word, error));
        return;
        }
    vector<double> aux = vocabError[word];
    for(int i = 0; i < error.size(); i++) {
        aux[i] += error[i];
    }
    vocabError[word] = aux;
}

void RNNParam::updateVocabError(unordered_map<string, vector<double>> left, unordered_map<string, vector<double>> right) {
    for (auto it = left.begin(); it != left.end(); ++it ) {
        this->updateVocabError(it->first, it->second);
    }

    for (auto it = right.begin(); it != right.end(); ++it ) {
        this->updateVocabError(it->first, it->second);
    }
}

void RNNParam::updateVocabError(unordered_map<string, vector<double>> left) {
    for (auto it = left.begin(); it != left.end(); ++it ) {
        this->updateVocabError(it->first, it->second);
    }
}

void RNNParam::updateTotalError(double x) {
    this->totalError += x;
}

vector<vector<double>> RNNParam::getSentimentWeightsMatrix() {
    return this->sentimentWeightsMatrix;
}

vector<vector<double>> RNNParam::getWeightsMatrix() {
    return this->weightsMatrix;
}

double RNNParam::getTotalError() {
    return this->totalError;
}

vector<double> RNNParam::getBiasSentimentMatrix() {
    return this->biasSentimentMatrix;
}

vector<double> RNNParam::getBiasWeightMatrix() {
    return this->biasWeightMatrix;
}

unordered_map<string, vector<double>> RNNParam::getVocabError() {
    return this->vocabError;
}



