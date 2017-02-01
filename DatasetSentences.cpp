#include "DatasetSentences.h"
#include <iostream>
#include<fstream>
#include<cstdio>
#include<string.h>
using namespace std;

/**
 * Map which keeps the reads from the "DatasetSentences.txt" stores only the sentences used for training
 * and their index.  One can find which senteces are used for testing by looking in datasetSplit.txt and
 * retrieve the sentences annotated with 1.
 */

DatasetSentences::DatasetSentences() {
    // Read from the file.
    //ifstream input("stanfordSentimentTreebank/datasetSentences.txt");
    ifstream input("Preprocessing.txt");
    ifstream input_datasplit("stanfordSentimentTreebank/datasetSplit.txt");
    ofstream outputFile;
    outputFile.open("debug.txt", std::ios_base::app);
    string line;
    string line_dataset;
    bool is_number = false;
    long long number = 0;
    string word = "";
    getline( input, line );
    getline( input_datasplit, line_dataset );
    while(getline( input, line ) ) {
         getline( input_datasplit, line_dataset );
         is_number = true;
         number = 0;
         word.clear();
         for(char & c : line) {
            if (c == '\t') {
                is_number = false;
            } else {
                if (!is_number) word +=c;
                else number = number*10 + c -'0';
            }
         }
         is_number = true;
         long long number1 = 0, number2 = 0;
         for(char & c : line_dataset) {
            if (c == ',') {
                is_number = false;

            } else {
                if (!is_number) number1 = number1*10 + c -'0';
                else number2 = number2*10 + c -'0';
            }
         }
         if (number == number2 && number1 == 1) {
         sentences.insert(make_pair(word, number));
         outputFile<<word<<" "<<number<<endl;
         }
    }
}

long long DatasetSentences::retrieveSentenceIndex(string phrase) {
    unordered_map<string, long long>::const_iterator found_iter = sentences.find(phrase);
    if (found_iter == sentences.end()) {
        ofstream outputFile;
        outputFile.open("debug.txt", std::ios_base::app);
        outputFile<<"The given phrase: "<<phrase<<" was not found in the sentence dataset."<<endl;
        return -1;
        }
    return found_iter->second;
}

unordered_map<string, long long> DatasetSentences::getSentencesMap() {
    return sentences;
}

