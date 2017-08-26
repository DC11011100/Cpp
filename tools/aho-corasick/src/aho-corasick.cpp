#include "../inc/aho-corasick.h"

Aho_Corasick::Trie::Trie(void)
{
    root = Node();
    state = root;
}

void Aho_Corasick::Trie::addPhrase(int index, const string &phrase, int weight)
{
    Node frontier = root;
    int match_i = 0;

    // Check if part of the phrase already exists in Trie
    while (state.matchPath.count(phrase[match_i]) != 0)
    {
        state = state.matchPath.at(phrase[match_i++]);
    }

    // Insert absent portion of phrase into Trie
    for (int newLetter = match_i; newLetter < phrase.length(); newLetter++)
    {
        Node newState;
        frontier.matchPath[phrase[newLetter]] = newState;
        frontier = newState;
    }

    // Reached the end of the word, add output
    Entry newPhrase = {phrase, weight};
    frontier.output[index] = newPhrase;;
}

void Aho_Corasick::Trie::buildForwards(const vector <string> &dict, const vector <int> &weights)
{
    for (int i=0; i<dict.size(); i++)
    {
        addPhrase(i, dict[i], weights[i]);
    }
}
