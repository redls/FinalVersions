#include <cstdio>
#include <random>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include<string>
#include "ParseASentence.h"
#include "MatrixOperations.h"
using namespace std;

bool Vocabulary::containsWord(string word) {
    unordered_map<string, vector<double>>::const_iterator found_iter = vocabulary.find(word);
    if (found_iter == vocabulary.end()) {
        return false;
        }
    return true;
}

Vocabulary::Vocabulary() {
    // Read from the file.
    ifstream input("stanfordSentimentTreebank/datasetSentences.txt");
    ofstream outputFile;
    outputFile.open("debugRNN.txt", std::ios_base::app);
    string line;
    string word = "";
    bool text = false;
    getline( input, line );
    while(getline( input, line ) ) {
        text = false;
        for(char & c : line) {
            if (c == '\t')  {
                    text = true;
                    continue;
                }
            if (text) {
                if (c ==  ' ') {
                    if (!containsWord(word)) {
                        vector<double> vec = createRandomDistributions(25);
                        //cout<<word<<" "<<endl;ſ
                        vocabulary.insert(make_pair(word, vec));
                    }
                    word.clear();
                } else {
                    word = word + c;
                }
            }
        }
        // For the last word.
        if (!containsWord(word)) {
                        vector<double> vec = createRandomDistributions(25);
                      //  cout<<word<<" "<<endl;
                        vocabulary.insert(make_pair(word, vec));
                    }
        word.clear();
    }

}

vector<double> Vocabulary::getWordRepresentation(string word) {
    vector<double> aux;
    unordered_map<string, vector<double>>::const_iterator found_iter = vocabulary.find(word);
    if (found_iter == vocabulary.end()) {
        return aux;
    }
    return found_iter->second;
}

void Vocabulary::addNewWord(string word) {
    if (this->containsWord(word)) return;
    vector<double> vec = createRandomDistributions(25);
    vocabulary.insert(make_pair(word, vec));
    return;
}

void Vocabulary::updateWordRepresentation(string word, vector<double> newRep, double learningRate){
    if (newRep.size() != 25) {
        cout<<" The new representation of the word vector does not have the same dimesions."<<endl;
        return;
    }
    for(int i = 0; i < newRep.size(); i++) {
        newRep[i] = newRep[i] * learningRate;
    }
  /*  cout<<"updating for the word: \n"<<word<<endl;
    cout<<"Word representation \n"<<endl;
    printElementsOfVector(vocabulary[word]);
    cout<<"Error representation"<< learningRate<<endl;
    printElementsOfVector(newRep);
    vocabulary[word] = substractTwoVectors(vocabulary[word], newRep);
    cout<<"updated word: \n"<<endl;
    printElementsOfVector(vocabulary[word]);*/
    vocabulary[word] = substractTwoVectors(vocabulary[word], newRep);
}


void Vocabulary::updateWordRepresentation(string word, vector<double> newRep, double learningRate, double regresionParam){
    if (newRep.size() != 25) {
        cout<<" The new representation of the word vector does not have the same dimesions."<<endl;
        return;
    }
    for(int i = 0; i < newRep.size(); i++) {
        newRep[i] = (newRep[i] + regresionParam * newRep[i]) * learningRate;
    }
  /*  cout<<"updating for the word: \n"<<word<<endl;
    cout<<"Word representation \n"<<endl;
    printElementsOfVector(vocabulary[word]);
    cout<<"Error representation"<< learningRate<<endl;
    printElementsOfVector(newRep);
    vocabulary[word] = substractTwoVectors(vocabulary[word], newRep);
    cout<<"updated word: \n"<<endl;
    printElementsOfVector(vocabulary[word]);*/
    vocabulary[word] = substractTwoVectors(vocabulary[word], newRep);
}
