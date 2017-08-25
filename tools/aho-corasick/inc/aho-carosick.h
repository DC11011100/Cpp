#include <unordered_map>

class Aho_Carosick
{
    class Trie
    {
        struct Node
        {
            unordered_map <int, int> weights;
            unordered_map <string, Node> children;


            bool isComplete(void);
        }

        public:
            void buildTrie(const vector<string> &dict);

        private:
            Node root;
            Node state;

            void buildForwards(const vector<string> &dict);
            void buildBackward(const vector<string> &dict);
    }
}
