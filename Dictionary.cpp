#include "Dictionary.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string.h>
using namespace std;

Dictionary::Dictionary() {
    ifstream input("stanfordSentimentTreebank/dictionary.txt");
    string line;
    bool is_number = false;
    long long number = 0;
    string word = "";
    while(getline( input, line ) ) {
         is_number = false;
         word.clear();
         for(char & c : line) {
            if (c == '|') {
                is_number = true;
                number = 0;
            } else {
                if (!is_number) word +=c;
                else number = number*10 + c -'0';
            }
         }
        dictionary.insert(make_pair(word, number));
    }
}


string addPhysicalBrackets(string phrase) {
    int leftBracket = phrase.find("-LRB-");
    while(leftBracket != string::npos) {
        phrase.replace(leftBracket, 5, "(");
        leftBracket = phrase.find("-LRB-");
    }
    int rightBracket = phrase.find("-RRB-");
    while(rightBracket != string::npos) {
        phrase.replace(rightBracket, 5, ")");
        rightBracket = phrase.find("-RRB-");
    }
    return phrase;
}


long long Dictionary::getPhraseIndex(string phrase2) {
    string phrase = addPhysicalBrackets(phrase2);
    unordered_map<string, long long>::const_iterator found_iter = dictionary.find(phrase);
    if (found_iter == dictionary.end()) {
        ofstream outputFile;
        outputFile.open("debug.txt", std::ios_base::app);
        outputFile<<"The given phrase: "<<phrase<<" was not found in the dictionary."<<endl;
        return -1;
        }
    return found_iter->second;
}
