#include <string>
#include <unordered_map>
#include <vector>
#include <queue>

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
            Node* failPath;
        } Node;

        class Trie
        {
            public:
                Trie(void);
                ~Trie(void);
                void buildTrie(const vector<Entry> &dict);
                int detect(int first, int last, char in);
                void reset(void);

            //private:
                Node *root;
                Node *state;

                void addPhrase(int index, const Entry &word);
                void buildForwards(const vector <Entry> &dict);
                void buildBackwards(void);
                void print(void);
        };
};
