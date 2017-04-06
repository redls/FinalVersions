#include<cstdio>
#include<string>
#include<vector>
#include<unordered_map>

using namespace std;

class RNNParam {
    vector<vector<double>> sentimentWeightsMatrix;
    vector<double> biasSentimentMatrix;

    vector<vector<double>> weightsMatrix;
    vector<double> biasWeightMatrix;

    unordered_map<string, vector<double>> vocabError;
    double totalError;

    public:
    RNNParam();
    RNNParam(int dimension);
    ~RNNParam();
    void resetFields();
    void resetVocab();
    void setSentimentWeightsMatrix(vector<vector<double>> x);
    void setTotalError(double x);
    void setWeightsMatrix(vector<vector<double>> x);
    void setBiasSentimentMatrix(vector<double> x);
    void setBiasWeightMatrix(vector<double> x);

    void updateSentimentWeightsMatrix(vector<vector<double>> x );
    void updateTotalError(double x);
    void updateWeightsMatrix(vector<vector<double>> x);
    void updateVocabError(string word, vector<double> error);
    void updateVocabError(unordered_map<string, vector<double>> left, unordered_map<string, vector<double>> right);
    void updateVocabError(unordered_map<string, vector<double>> left);
    void updateBiasSentimentMatrix(vector<double> x);
    void updateBiasWeightMatrix(vector<double> x);

    vector<vector<double>> getSentimentWeightsMatrix();
    vector<vector<double>> getWeightsMatrix();
    double getTotalError();
    unordered_map<string, vector<double>> getVocabError();
    vector<double> getBiasSentimentMatrix();
    vector<double> getBiasWeightMatrix();
};
