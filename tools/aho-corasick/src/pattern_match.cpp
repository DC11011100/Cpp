#include <iostream>
#include "../inc/aho-corasick.h"
#include <fstream>


using namespace std;
int main(int argc, char *argv[])
{
    // Read dictionary
    vector <Aho_Corasick::Entry> dict;
    if (argc < 2 || argc > 2) fprintf(stderr, "\n[Usage]: %s dict.txt\n\n", argv[0]);
    else
    {
        ifstream dictFile(argv[1]);
        string phrase;
        int weight;
        while (dictFile >> phrase  >> weight) 
        {
            Aho_Corasick::Entry newWord = {phrase, weight};
            dict.push_back(newWord);
        }
    }

    // Build the automaton
    Aho_Corasick::Trie dictMachine;
    dictMachine.buildForwards(dict);
    dictMachine.buildBackwards();

    // Show tree
    printf("\n Tree \n");
    dictMachine.print();
    return 0;
}


