#lhy
#2016.1

import os
import cPickle

articleDict = {}
wordDict = {}

def get_item(line):
    global articleDict
    return "%d %s" % (articleDict[line.split('\t')[0]],line.split('\t')[1])

def get_vector(word):
    global wordDict
    word_index = wordDict[word]
    wordHandle = open(r'../../wikiprep-esa/vectors/%s' % word)
    in_list = wordHandle.readlines()
    vector = map(get_item,in_list)
    vector = map((lambda x:"E%d %s" % (word_index,x)),vector)
    return ''.join(vector)

inHandle = open(r'../data/enwiki.txt')
words = set(inHandle.read().lower().split(' '))
wordList = []

for root,dir,filenames in os.walk("../../wikiprep-esa/vectors"):
    for fn in filenames:
        filename = fn.lower()
        n = len(open(r'../../wikiprep-esa/vectors/%s' % fn).readlines())
        if filename in words and n > 0:
            wordList.append(filename)
for i in range(len(wordList)):
    wordDict[wordList[i]] = i
cPickle.dump(wordList,open("../data/wordList_ESA",'wb'))

articleHandle = open(r'../../wikiprep-esa/selected.txt')
in_list = articleHandle.readlines()
articles = map((lambda x:x.strip('\n')),in_list)
for i in range(len(articles)):
    articleDict[articles[i]] = i

outHandle = open("../data/ESA_MATRIX.txt",'w')
outHandle.write("%d %d %d\n" % (len(wordList),len(wordList),171332))
text = ''.join(map(get_vector,wordList))
outHandle.write(text)

inHandle.close()
outHandle.close()
