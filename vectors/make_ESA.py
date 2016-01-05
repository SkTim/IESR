#lhy
#2015.12

import sys
import cPickle
import numpy as np

articleDict = {}

def get_wordsim(dataset):
    wordSim = set([])
    inHandle = open("../../wordsim/%s/combined.csv" % dataset)
    in_list = inHandle.readlines()
    for i in range(1,len(in_list)):
        l = in_list[i].split(',')
        wordSim.add(l[0].lower())
        wordSim.add(l[1].lower())
    return list(wordSim)

def article_vector(word):
    global articleDict
    try:
        ESAHandle = open("../../wikiprep-esa/vectors/%s" % word,'r')
    except:
        return ""
    vector = ['0'] * 171332
    ESAList = ESAHandle.readlines()
    ESAVector = map((lambda x:[articleDict[x.split('\t')[0]],x.split('\t')[1].strip('\n')]),ESAList)
    for item in ESAVector:
        vector[item[0]] = item[1]
    return "%s %s\n" % (word,' '.join(vector))

inHandle = open(r'../../wikiprep-esa/selected.txt')
wordList = get_wordsim(sys.argv[1])
articleList = map((lambda x:x.strip('\n')),inHandle.readlines())
for i in range(len(articleList)):
    articleDict[articleList[i]] = i
outHandle = open("../data/text_ESA_%s.txt" % sys.argv[1],'w')
ESAVectors = ''.join(map(article_vector,wordList))
outHandle.write("%s 171332\n" % len(wordList))
outHandle.write(ESAVectors)
inHandle.close()
outHandle.close()
