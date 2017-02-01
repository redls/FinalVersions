#include<cstdio>
#include<string>
#include<vector>
#include<unordered_map>

using namespace std;

class RNNParam {
    vector<vector<double>> sentimentWeightsMatrix;
    vector<vector<double>> weightsMatrix;
    unordered_map<string, vector<double>> vocabError;

    public:
    RNNParam();
    ~RNNParam();
    void setSentimentWeightsMatrix(vector<vector<double>> x);
    void updateSentimentWeightsMatrix(vector<vector<double>> x );
    void setWeightsMatrix(vector<vector<double>> x);
    void updateWeightsMatrix(vector<vector<double>> x);
    void updateVocabError(string word, vector<double> error);
    void updateVocabError(unordered_map<string, vector<double>> left, unordered_map<string, vector<double>> right);

    vector<vector<double>> getSentimentWeightsMatrix();
    vector<vector<double>> getWeightsMatrix();
    unordered_map<string, vector<double>> getVocabError();
};
