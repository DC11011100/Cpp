#include "../inc/aho-carosick.h"

Aho_Carosick::Trie Trie(void)
{
    root = Node();
    state = root;
}

void Aho_Carosick::addPhrase(int index, const string &phrase, int weight)
{
    Node frontier = root;
    int match_i = 0;

    // Check if part of the phrase already exists in Trie
    while (state.matchPath(phrase[match_i]))
    {
        state = state.matchPatch.at(phrase[match_i++]);
    }

    // Insert absent portion of phrase into Trie
    for (int newLetter = match_i; newLetter < phrase.length(); newLetter++)
    {
        Node newState();
        frontier.matchPath.insert( {phrase[newLetter], newState} );
        frontier = newState;
    }

    // Reached the end of the word, add output
    Entry newPhrase = {phrase, weight};
    frontier.output.insert( {index, newPhrase} );
}

void buildForwards(const vector <string> &dict)
{
    for (string phrase : dict)
    {
        addPhrase(phrase);
    }
}
