#include <iostream>
#include "../inc/aho-corasick.h"
#include <fstream>


using namespace std;
int main(int argc, char *argv[])
{
    int first = 0;
    int last = 0;
    int dictArg = 0;

    // Ensure valid input
    if (argc == 2)
    {
        dictArg = 1;
    } else if (argc == 4)
    {
        first = atoi(argv[1]);
        last  = atoi(argv[2]);
        dictArg = 3;
    } else
    {
        fprintf(stderr, "\n[Usage]: %s start end dict.txt\n\n", argv[0]); // [start, end] inclusive!
        fprintf(stderr, "\n[Usage]: %s dict.txt\n\n", argv[0]); // [start, end] inclusive!
        return 1;
    }
 

    // Read in the dictionary
    vector <Aho_Corasick::Entry> dict;
    ifstream dictFile(argv[dictArg]); 
    
    string phrase;
    int weight;
    while (dictFile >> phrase  >> weight) 
    {
        Aho_Corasick::Entry newWord = {phrase, weight};
        dict.push_back(newWord);
    } 
    if (last == 0) last = dict.size();


    // Build the automaton
    Aho_Corasick::Trie dictMachine;
    dictMachine.buildTrie(dict);

    printf("Starting machine...\n");
    string input;
    while (1)
    {    
        cin >> input;
        if (cin.fail()) break;

        for (char c : input)
        {
            dictMachine.detect(first, last, c);
        }
        dictMachine.reset();

    }

    return 0;
}


