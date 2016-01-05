#lhy
#2015.11

import cPickle

wordList = cPickle.load(open("../data/wordList_ESA",'rb'))
wordList = map((lambda x:x.strip('\n')),wordList)
inHandle = open(r'../data/wordVectors_8')
in_list = inHandle.readlines()
outHandle = open("../data/text8.txt",'w')
for (i,line) in enumerate(in_list):
    if i == 0:
        outHandle.write(in_list[i])
        continue
    if not wordList[i - 1].isalpha():
        continue
    if '\n' in wordList[i - 1]:
        continue
    outHandle.write("%s %s" % (wordList[i - 1].strip(' '),in_list[i]))
outHandle.close()
