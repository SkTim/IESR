#lhy
#2015.5

import cPickle
import word2vec
import math
import analysis

#inHandle = open(r'../../nnse/text8')
#text = inHandle.read()
#wordDict = {}
#wordList = text.split(' ')

#for word in wordList:
#    if word in wordDict:
#        wordDict[word] += 1
#    else:
#        wordDict[word] = 1

#wordSet = set([])
#cPickle.dump(wordSet,open("wordSet",'wb'))
#for (key,value) in wordDict.items():
#    if value > 10 and len(key) > 3:
#        wordSet.add(key)

model = word2vec.load("../data/article8.txt")
words = list(model.vocab)

vectors = {}
for i in range(1,len(words)):
    #if words[i] not in wordSet:
    #    continue
    v = model[words[i]]
    vectors[words[i]] = list(v)
print "Vectors ok"

myAnalysis = analysis.Analysis(vectors)
myAnalysis.dimension_analysis()

#cPickle.dump(vectors,open("../vectors/vectors",'wb'))
print "Data ok"
