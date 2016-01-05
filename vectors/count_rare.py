#lhy
#2015.12

import sys
import cPickle

wordList = cPickle.load(open("../data/wordList_test",'rb'))
inHandle = open(r'../../wordsim/%s/combined.csv' % sys.argv[1])
in_list = inHandle.readlines()
wordSim = set([])
for i in range(1,len(in_list)):
    l = in_list[i].split(',')
    wordSim.add(l[0])
    wordSim.add(l[1])
print len(wordSim)
n = 0
wordSet = set(wordList)
for word in wordSim:
    if word not in wordSet:
        n += 1
print n
