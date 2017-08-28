## Linear Substring Searching
This project is a tool I developed for my own future use. After reading a dictionary, *pattern_match* takes in user keyboard input while recognizing any dictionary word in linear time. Everytime a word is discovered its payload is added to the total score which is then displayed. It does this by using the inluded libraries which contains my variant of the Aho-Corasick algorithm. Read more
<a href ="https://pdfs.semanticscholar.org/3547/ac839d02f6efe3f6f76a8289738a22528442.pdf">here</a>

## Building

## Running
After building run the program as such:
```
$ pattern_match dict.txt
```
where *dict.txt* is a dictionary containing entries of the form:
```
iamaword 24
secondword 53
coffee 99
```
Note entries are of type {phrase, payload}
## 