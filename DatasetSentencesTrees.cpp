#include<cstdio>
#include<vector>
#include<unordered_map>
#include<iostream>
#include<fstream>
#include<string.h>

#include "DatasetSentenceTrees.h"
#include "Tree.h"

using namespace std;

/**
 * Map which stores only the trees of sentences and the sentence's index.
 */

DatasetSentencesTrees::DatasetSentencesTrees() {
    // Read from the file.
    ifstream input("stanfordSentimentTreebank/STree.txt");
    ifstream input_datasplit("stanfordSentimentTreebank/SOSTr.txt");
    ofstream outputFile;
    outputFile.open("debug.txt", std::ios_base::app);
    string line;
    string line_dataset;
    long long number = 0;
    long long counter = 0;
    string word = "";
    while(getline( input, line ) ) {
         counter++;
         //getline( input_datasplit, line_dataset );
         trees.insert(make_pair(counter, line));
    }
}

// Given an index retrieve the string containing the tree representation.
string DatasetSentencesTrees::retrieveSentenceIndex(long long index) {
    unordered_map<long long, string>::const_iterator found_iter = trees.find(index);
    if (found_iter == trees.end()) {
        ofstream outputFile;
        outputFile.open("debug.txt", std::ios_base::app);
        cout<<"The given index: "<<index<<" was not found in the trees dataset."<<endl;
        return "";
        }
    return found_iter->second;
}
