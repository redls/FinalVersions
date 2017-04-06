#include <cstdio>
#include <random>
#include <fstream>
#include <iostream>
#include <ctype.h>
#include<string>
#include "ParseASentence.h"
#include "MatrixOperations.h"
using namespace std;

#define WORD_SIZE_VEC 30

bool Vocabulary::containsWord(string word) {
    unordered_map<string, vector<double>>::const_iterator found_iter = vocabulary.find(word);
    if (found_iter == vocabulary.end()) {
        return false;
        }
    return true;
}

Vocabulary::~Vocabulary(){}


Vocabulary::Vocabulary() {
    // Read from the file.
   // ifstream input("stanfordSentimentTreebank/datasetSentences.txt");
    ifstream input("Preprocessing.txt");
    ifstream input_datasplit("stanfordSentimentTreebank/datasetSplit.txt");
    ofstream outputFile;
    outputFile.open("debugRNN.txt", std::ios_base::app);
    string originalLine;
    string line;
    string line_dataset;
    string word = "";
    long long indexOfSentence = 0;
    bool text = false;
    getline(input, originalLine);
    getline(input_datasplit,line_dataset);
    while(getline(input, originalLine)) {
        getline(input_datasplit, line_dataset);
        line = originalLine + " ";
        text = false;
        indexOfSentence = 0;
        for(char & c : line) {
            if (c == '\t')  {
                    text = true;
                    bool is_number = true;
                    long long number2= 0, number1 = 0;
                    for(char & c : line_dataset) {
                       if (c == ',') {
                           is_number = false;

                       } else {
                           if (!is_number) number2 = number2*10 + c -'0';
                           else number1 = number1*10 + c -'0';
                       }
                    }
                   // cout<<indexOfSentence<<" "<< number1<<" "<<number2<<endl;
                    if (indexOfSentence == number1 && number2 != 1) {
                        break;
                    }
                    continue;
                }
            if (!text) {
                indexOfSentence = indexOfSentence*10 + c -'0';
            }
            if (text) {
                if (c ==  ' ') {
                    if (!containsWord(word)) {
                        vector<double> vec = createRandomDistributions(WORD_SIZE_VEC);
                        //cout<<word<<" "<<endl;ſ
                        vocabulary.insert(make_pair(word, vec));
                    }
                    word.clear();
                } else {
                    char aux = tolower(c);
                    word = word + aux;
                }
            }
        }
        // For the UNKNOWN word.
        vector<double> vec = createRandomDistributions(WORD_SIZE_VEC);
        vocabulary.insert(make_pair("UNKNOWN", vec));
        word.clear();
    }
    cout<<vocabulary.size()<<"vocab size"<<endl;

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
    vector<double> vec = createRandomDistributions(WORD_SIZE_VEC);
    vocabulary.insert(make_pair(word, vec));
    return;
}


void Vocabulary::updateWordRepresentation(string word, vector<double> newRep){

    vocabulary[word] = newRep;
}

void Vocabulary::updateWordRepresentation(string word, vector<double> newRep, double learningRate){
    if (newRep.size() != WORD_SIZE_VEC) {
        cout<<" The new representation of the word vector does not have the same dimesions."<<endl;
        return;
    }
    for(int i = 0; i < newRep.size(); i++) {
        newRep[i] = newRep[i] * learningRate;
    }
    vocabulary[word] = substractTwoVectors(vocabulary[word], newRep);
}


void Vocabulary::updateWordRepresentation(string word, vector<double> newRep, double learningRate, double regresionParam){
    if (newRep.size() != WORD_SIZE_VEC) {
        cout<<" The new representation of the word vector does not have the same dimesions."<<endl;
        return;
    }
    vector<double> lala = newRep;
    for(int i = 0; i < newRep.size(); i++) {
        newRep[i] = (newRep[i] + regresionParam * newRep[i]) * learningRate;

    }
    vector<double> temp = substractTwoVectors(vocabulary[word], newRep);
    for(int i = 0;i < temp.size(); i++) {
            if (std::isnan((double)temp[i])) {
                cout<<"NAN is located in updating word rep"<<endl;
                printElementsOfVector(temp);
                cout<<endl<<endl;
                printElementsOfVector(newRep);
                cout<<endl<<endl;
                printElementsOfVector(vocabulary[word]);
                 cout<<endl<<endl;
                printElementsOfVector(lala);
                exit(0);
            }
        }

    vocabulary[word] = substractTwoVectors(vocabulary[word], newRep);
}

vector<vector<double>> Vocabulary::getMatrixFromVocabulary() {
     vector<vector<double>> result;
     for(auto j = vocabulary.begin(); j != vocabulary.end(); ++j) {
                result.push_back(j->second);
            }
     return result;
}
