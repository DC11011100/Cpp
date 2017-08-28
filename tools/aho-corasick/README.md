## Linear Substring Searching
This project is a tool I developed for my own future use. After reading a dictionary, *pattern_match* takes in user keyboard input while recognizing any dictionary word in linear time. Everytime a word is discovered its payload is added to the total score which is then displayed. It does this by using the inluded libraries which contains my variant of the Aho-Corasick algorithm. Read more
<a href ="https://pdfs.semanticscholar.org/3547/ac839d02f6efe3f6f76a8289738a22528442.pdf">here</a>

## Running
After building, you can run the program in two ways:
```
$ pattern_match dict.txt
$ pattern_match $START $END dict.txt
```
The first method uses the entire dictionary, while the second inclusively uses words indexed from $START to $END (0-indexed). 

*Note*: dict.txt is a path to a dictionary containing entries of the form:
```
iamaword 24
secondword 53
coffee 99
```
Note entries are of type {phrase, payload}

## Notes
 * Building the Trie is O(c) where 'c' is the sum of the lengths of the words in the dictionary.
 * Searching a string using a prebuilt Trie takes O(s) where 's' is the length the of the string.

