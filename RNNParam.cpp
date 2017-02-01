#include<cstdio>
#include<string>
#include<unordered_map>
#include<iostream>
#include "RNNParam.h"
#include "MatrixOperations.h"

using namespace std;


RNNParam::RNNParam() {
    sentimentWeightsMatrix.clear();
    weightsMatrix.clear();
    vector<double> aux = getZeros(50);
    for(int i = 0; i < 25; i++) weightsMatrix.push_back(aux);
    vector<double> aux2 = getZeros(25);
    sentimentWeightsMatrix.push_back(aux2);
    sentimentWeightsMatrix.push_back(aux2);
    vocabError.clear();

}

void RNNParam::setSentimentWeightsMatrix(vector<vector<double>> x) {
    this->sentimentWeightsMatrix = x;
}

void RNNParam::updateSentimentWeightsMatrix(vector<vector<double>> x) {
    if (x.size() != sentimentWeightsMatrix.size() || x[0].size() != sentimentWeightsMatrix[0].size()) {
        cout<<"Sentiment matrix couldn't be updated due to different dimensions. (updateSentimentWeightsMatrix())."<<endl;
        exit(0);
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
        exit(0);
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

vector<vector<double>> RNNParam::getSentimentWeightsMatrix() {
    return this->sentimentWeightsMatrix;
}

vector<vector<double>> RNNParam::getWeightsMatrix() {
    return this->weightsMatrix;
}

unordered_map<string, vector<double>> RNNParam::getVocabError() {
    return this->vocabError;
}
