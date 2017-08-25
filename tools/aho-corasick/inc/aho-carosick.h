#include <unordered_map>

class Aho_Carosick
{

    typedef struct Entry
    {
        string phrase;
        int    weight;
    } Entry;

    typedef struct Node
    {
        unordered_map <int, Entry> output;
        unordered_map <char, Node> matchPath;
        unordered_map <char, Node> failPath;
    } Node;

    class Trie
    {
        public:
            Trie(void);
            void buildTrie(const vector<string> &dict);

        private:
            Node root;
            Node state;

            void addPhrase(int index, const string &phrase, int weight);
            void buildForwards(const vector<string> &dict);
            void buildBackward(const vector<string> &dict);
    }
    

}
