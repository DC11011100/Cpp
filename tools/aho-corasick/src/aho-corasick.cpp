#include "../inc/aho-corasick.h"
#include <iostream>

// O(1)
Aho_Corasick::Trie::Trie(void)
{
    root = new Node;
    root->failPath = root;
    state = root;
}

// O(n*w)
// where 'n' is the number of entrys in the dictionary and 'w' the max word length in the dictionary
// One could say this is linear with respect to the sum of the lengths of the words -- See linked paper
void deleteHelper(Aho_Corasick::Node *n)
{
    // Delete all children
    for (auto i=n->matchPath.begin(); i != n->matchPath.end(); i++)
    {
        deleteHelper(i->second);
    }

    delete n;
}

// O(n*w)
Aho_Corasick::Trie::~Trie(void)
{
    deleteHelper(root);
}

// O(w)
// where 'w' is the length of the word.
void Aho_Corasick::Trie::addPhrase(int index, const Entry &word)
{
    int match_i = 0;

    Node *cursor = root;
    // Check if part of the phrase already exists in Trie
    while (cursor->matchPath.count(word.phrase[match_i]) != 0)
    {
        cursor = cursor->matchPath.at(word.phrase[match_i++]);
    }

    // Insert absent portion of phrase into Trie
    for (int newLetter = match_i; newLetter < word.phrase.length(); newLetter++)
    {
        Node* frontier = new Node;
        cursor->matchPath[word.phrase[newLetter]] = frontier;
        cursor = frontier;
    }

    // Reached the end of the word, add output
    Entry newPhrase = {word.phrase, word.weight};
    cursor->output[index] = newPhrase;
}

// O(n*w)
void Aho_Corasick::Trie::buildForwards(const vector <Entry> &dict)
{
    for (int i=0; i<dict.size(); i++)
    {
        addPhrase(i, dict[i]);
    }
}

// O(n*w)
void Aho_Corasick::Trie::buildBackwards(void)
{
    // Set nodes of depth one for failPath --> root and queue all children
    queue <Node*> q;
    for (auto i=root->matchPath.begin(); i != root->matchPath.end(); i++)
    {
        q.push(i->second);
        i->second->failPath = root;
    }

    // Set failPaths breadth-first
    while (!q.empty())
    {
        Node *parent = q.front();
        q.pop();

        for (auto i=parent->matchPath.begin(); i != parent->matchPath.end(); i++)
        {
            char letter = i->first;
            Node *child = i->second;
            q.push(child);
            
            // Find the end of a matching prefix
            Node *dest  = parent->failPath;
            while (dest->matchPath.count(letter) == 0 && dest != root) dest = dest->failPath;

            // Set the fail path
            if (dest->matchPath.count(letter) == 0)
            {
                child->failPath = root;
            } else 
            {
                child->failPath = dest->matchPath[letter];
            }

            // Merge outputs
            child->output.insert(child->failPath->output.begin(), child->failPath->output.end());
        }
    }
}


// O(n*w)
void Aho_Corasick::Trie::buildTrie(const vector<Entry> &dict)
{
    buildForwards(dict);
    buildBackwards();
}


// O(1)
long long Aho_Corasick::Trie::detect(int first, int last, char in)
{
    long long total = 0;
    // Traverse to a matching point.
    while (state->matchPath.count(in) == 0 && state != root)
    {
        state = state->failPath;
    }

    // Set next state
    if(state->matchPath.count(in) == 0) state = root;
    else
    {
        state = state->matchPath[in];
    }

    for (auto i=state->output.begin(); i != state->output.end(); i++)
    {
        int index = i->first;
        Entry word = i->second;

        if (index >= first && index <= last)
        {
            total += word.weight;
            //cout << "dict[" << index << "]: " << word.phrase << " --> " << word.weight << endl;
        }
    }
    return total;
}

// O(1)
void Aho_Corasick::Trie::reset(void)
{
    state = root;
}


// O(n*w)
void printHelper(const Aho_Corasick::Node *n)
{
    printf("[0x%lX] : {", (unsigned long)n);
    if (!n) return;

    for (auto child = n->matchPath.begin(); child != n->matchPath.end(); child++)
    {
        if (child == n->matchPath.begin())
        {
            printf("(%c, 0x%lX)", child->first, (unsigned long)child->second);
        } else
        {
            printf(" | (%c, 0x%lX)", child->first, (unsigned long)child->second);
        }
    }
    printf("} FAIL: 0x%lX\n", (unsigned long)n->failPath);

    for (auto child=n->matchPath.begin(); child != n->matchPath.end(); child++)
    {
        printHelper(child->second);
    }
}

// O(n*w)
void Aho_Corasick::Trie::print(void)
{
    printf("ROOT:");
    printHelper(root);
}


