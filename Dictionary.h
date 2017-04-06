#include <unordered_map>
#include <cstdio>

using namespace std;

/*
 * Maps phrases with a unique index. Using this index, one can retrieve the
 * sentiment score for the specific phrase.
 */
class Dictionary {
    private:
    unordered_map<string, long long> dictionary;

    public:

    Dictionary();
    ~Dictionary();

    long long getPhraseIndex(string phrase);

};
