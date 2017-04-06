#include<cstdio>
#include<iostream>
#include<fstream>
#include <ctype.h>
#include<string.h>
#include "Preprocessing.h"
using namespace std;

// Reads from the SOStr.txt file and rewrites the dataset file, in order to avoid the reinterpretation of accents
// and paranthesis.
void preprocessSentences() {
    ifstream input("stanfordSentimentTreebank/SOStr.txt");
    ofstream outputResult("Preprocessing.txt");
    outputResult<<"sentence_index	sentence"<<endl;
    long long index = 1;
    string line;
    string word = "";
    while(getline(input, line)) {
        word = "";
        outputResult<<index<<"\t";
        for(char &c: line) {
            if (c == '|') {
                outputResult<<word<<" ";
                word = "";
            } else {
                    char aux = tolower(c);
                    word +=aux;
            }
        }
        outputResult<<word;
        outputResult<<endl;
        index++;
    }

}
