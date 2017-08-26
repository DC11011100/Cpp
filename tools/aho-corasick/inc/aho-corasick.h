#include <string>
#include <unordered_map>
#include <vector>

using namespace std;
class Aho_Corasick
{
    public:
        typedef struct Entry
        {
            string phrase;
            int    weight;
        } Entry;
        
        typedef struct Node
        {
            unordered_map <int, Entry> output;
            unordered_map <char, Node*> matchPath;
            unordered_map <char, Node*> failPath;
        } Node;

        class Trie
        {
            public:
                Trie(void);
                void buildTrie(const vector<Entry> &dict);

            //private:
                Node *root;
                Node *state;

                void addPhrase(int index, const Entry &word);
                void buildForwards(const vector <Entry> &dict);
                void buildBackward(const vector <string> &dict);
                void print(void);
        };
};
