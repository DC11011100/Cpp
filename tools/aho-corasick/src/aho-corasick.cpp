#include "../inc/aho-corasick.h"

Aho_Corasick::Trie::Trie(void)
{
    root = new Node;
    state = root;
}

void Aho_Corasick::Trie::addPhrase(int index, const Entry &word)
{
    int match_i = 0;

    // Check if part of the phrase already exists in Trie
    while (state->matchPath.count(word.phrase[match_i]) != 0)
    {
        state = state->matchPath.at(word.phrase[match_i++]);
    }

    // Insert absent portion of phrase into Trie
    for (int newLetter = match_i; newLetter < word.phrase.length(); newLetter++)
    {
        Node* frontier = new Node;
        state->matchPath[word.phrase[newLetter]] = frontier;
        state = frontier;
    }

    // Reached the end of the word, add output
    Entry newPhrase = {word.phrase, word.weight};
    state->output[index] = newPhrase;
    state = root;
}

void Aho_Corasick::Trie::buildForwards(const vector <Entry> &dict)
{
    for (int i=0; i<dict.size(); i++)
    {
        addPhrase(i, dict[i]);
    }
}

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
    printf("}\n");

    for (auto child=n->matchPath.begin(); child != n->matchPath.end(); child++)
    {
        printHelper(child->second);
    }
}

void Aho_Corasick::Trie::print(void)
{
    printf("ROOT:");
    printHelper(root);
}


