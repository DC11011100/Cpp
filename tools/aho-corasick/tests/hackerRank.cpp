#include "../inc/aho-corasick.h"
#include <iostream>
#include <fstream>

using namespace std;
int main(){
    ifstream inputFile("input31.txt");

    int n;
    inputFile >> n;;
    vector<string> genes(n);
    for(int genes_i = 0; genes_i < n; genes_i++){
       inputFile >> genes[genes_i];
    }
    vector<int> health(n);
    for(int health_i = 0; health_i < n; health_i++){
       inputFile >> health[health_i];
    }
    
    // Build dictionary
    vector<Aho_Corasick::Entry> dict(n);
    for(int i=0; i<genes.size(); i++)
    {
        dict[i].phrase = genes[i];
        dict[i].weight = health[i];
    }
    
    // Build automaton
    Aho_Corasick::Trie dictMachine;
    dictMachine.buildTrie(dict);

    printf("Done building\n");
    
    
    
    int s;
    long long min = 0;
    long long max = 0;
    inputFile >> s;
    cout << "queries = " << s;
    for(int a0 = 0; a0 < s; a0++)
    {
        int first;
        int last;
        string d;
        inputFile >> first >> last >> d;
        
        // Run dictionary search on string O(stringLength)
        dictMachine.reset();
        long long sum = 0;
        for (char c : d)
        {
            sum += dictMachine.detect(first, last, c);
        }
        
        if (a0 == 0)
        {
            max = sum;
            min = sum;
        }
        
        // Check for new extrema
        if (max < sum) max = sum;
        if (min > sum) min = sum;
    }    
    printf("%lld %lld\n", min, max);
    
    return 0;
}

