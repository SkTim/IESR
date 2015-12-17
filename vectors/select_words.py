#lhy
#2015.10

import cPickle

wordList_wiki = cPickle.load(open("../data/wordList_wiki",'rb'))
wordList = []
inHandle = open(r'../data/enwiki.txt')
text = inHandle.read()
words = set(text.split(' '))
for word in wordList_wiki:
    if word in words:
        wordList.append(word)
cPickle.dump(wordList,open("../data/wordList",'wb'))
