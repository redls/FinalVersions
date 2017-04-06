#include "TestsetSentences.h"
#include <iostream>
#include<fstream>
#include<cstdio>
#include<ctype.h>
#include<string.h>
using namespace std;

/*
 * Map which keeps the reads from the "DatasetSentences.txt" stores only the sentences used for testing
 * and their index. One can find which senteces are used for testing by looking in datasetSplit.txt and
 * retrieve the sentences annotated with 2.
 */

TestsetSentences::TestsetSentences() {
    // Read from the file.
    //ifstream input("stanfordSentimentTreebank/datasetSentences.txt");
    ifstream input("Preprocessing.txt");
    ifstream input_datasplit("stanfordSentimentTreebank/datasetSplit.txt");
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
                if (!is_number) {
                    char aux = tolower(c);
                    word += aux;
                    }
                else number = number*10 + c -'0';
            }
         }
         is_number = true;
         long long number1 =0, number2 = 0;
         for(char & c : line_dataset) {
            if (c == ',') {
                is_number = false;

            } else {
                if (is_number) number1 = number1*10 + c -'0';
                else number2 = number2*10 + c -'0';
            }
         }
         if (number == number1 && number2 == 2) {
            sentences.insert(make_pair(word, number));
            }
    }
}

unordered_map<string, long long> TestsetSentences::getSentencesMap() {
    return sentences;
}
