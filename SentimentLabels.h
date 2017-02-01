#include <unordered_map>
#include<cstdio>

using namespace std;

/*
 * Maps phrase indexes with their sentiment score.
 */

class SentimentLabels {
    private:
    unordered_map<long long, double> sentimentLabelsMap;

    public:
    SentimentLabels();
    double getSentimentScore(long long phraseIndex);

};
