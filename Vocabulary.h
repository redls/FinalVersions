#include<cstdio>
#include<unordered_map>
#include<vector>

using namespace std;

/**
 *  Class which keeps track of the words that appear in the data set.
 *  It assings a vector to each word.
 */

class Vocabulary {
    unordered_map<string, vector<double>> vocabulary;

    public:
        Vocabulary();
        ~Vocabulary();
        vector<double> getWordRepresentation(string word);
        bool containsWord(string word);
        void addNewWord(string word);
        void updateWordRepresentation(string word, vector<double> newRep);
        void updateWordRepresentation(string word, vector<double> newRep, double learningRate);
        void updateWordRepresentation(string word, vector<double> newRep, double learningRate, double regresionParam);
        vector<vector<double>> getMatrixFromVocabulary();
};
