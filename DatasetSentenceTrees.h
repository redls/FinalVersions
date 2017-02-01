#include<cstdio>
#include<vector>
#include<unordered_map>
#include<string.h>

using namespace std;

/**
 * Map which stores only the trees of sentences and the sentence's index.
 */

 class DatasetSentencesTrees {
    private:
        unordered_map<long long, string> trees;

    public:

        // Constructor.
        DatasetSentencesTrees();
        // Given an index retrieve the string containing the tree representation.
        string retrieveSentenceIndex(long long index);
 };

