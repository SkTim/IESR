#lhy
#2015.2

import cPickle
#import spams
import numpy as np

from scipy import sparse

def _log(x):
    if x < 1:
        return 0.0
    else:
        return np.log(x)

class Matrix():

    def __init__(self,window_size,filename):
        self.window_size = window_size
        self.inHandle = open(filename,'r')
        self.in_data = self.inHandle.read()

    def getDict(self):
        self.words = self.in_data.split(' ')
	del self.in_data
        self.words = self.words[1:]
        self.worddict = {}
	#wordSet = set(self.wordlist)
        for word in self.words:
	    #if word not in self.wordSet:
		#continue
            if word in self.worddict:
                self.worddict[word] += 1
            else:
                self.worddict[word] = 1
        rareWords = set([])
        for word in self.worddict:
            if self.worddict[word] < 5:
		rareWords.add(word)
	for word in rareWords:
            del(self.worddict[word])
        return self.worddict

    def getIndexDict(self):
        self.wordList = self.worddict.keys()
        self.N = len(self.wordList)
        cPickle.dump(self.wordList,open("../data/wordList8_parsed_1",'wb'))

    def getInfoMatrix(self):
        self.infoMatrix = map((lambda x:{}),self.wordList)
        #index1 = {}
	index2 = {}
	#for i in range(len(self.wordlist)):
	#    index1[self.wordlist[i]] = i
        for i in range(len(self.wordList)):
            index2[self.wordList[i]] = i
        l = sorted(self.worddict.iteritems(),key = lambda x:x[1],reverse = True)
        outHandle = open("../data/vocabs.txt",'w')
        outHandle.write("%d\n" % len(l))
        outHandle.write('\n'.join(map((lambda x:"%d %d" % (index2[x[0]],x[1])),l)))
        outHandle.close()
        for i in range(len(self.words)):
            if self.words[i] not in index2:
                continue
            currentID = index2[self.words[i]]
            for j in range(-self.window_size,self.window_size):
                if i + j < 0:
                    continue
                if i + j >= len(self.words):
                    break
                if self.words[i + j] not in self.worddict:
                    continue
                contextID = index2[self.words[i + j]]
                if contextID in self.infoMatrix[currentID]:
                    self.infoMatrix[currentID][contextID] += 1
                else:
                    self.infoMatrix[currentID][contextID] = 1
        self.infoMatrix = map((lambda x:x.items()),self.infoMatrix)

    def PPMI(self):
        length = len(self.words)
        del self.words
        l = len(self.wordList)
        for i in range(l):
            a = float(length) / self.worddict[self.wordList[i]]
            self.infoMatrix[i] = map((lambda x:(x[0],_log(x[1] * a / float(self.worddict[self.wordList[x[0]]])))),self.infoMatrix[i])

    def writeMatrix(self,filename):
        inHandle = open(r'../../wikiprep-esa/selected.txt')
        lines = inHandle.readlines()
        articles = map((lambda x:x.strip('\n')),lines)
        articleDict = {}
        for i in range(len(articles)):
            articleDict[articles[i]] = i
        outHandle = open(filename,'w')
        outHandle.write("%d %d 171332\n" % (len(self.wordList),len(self.wordList)))
        l = len(self.wordList)
        for i in range(l):
            outHandle.write("PPMI:\n")
            pl = map((lambda x:"%d %d %f" % (i,x[0],x[1])),self.infoMatrix[i])
            outHandle.write("%s\n" % '\n'.join(pl))
	    outHandle.write("ESA:\n")
            try:
                inHandle = open(r'../../wikiprep-esa/vectors/%s' % self.wordList[i])
            except:
                continue
            text = inHandle.readlines()
            esa_lines = map((lambda x:x.split('\t')),text)
            el = map((lambda x:"%d %s %s" % (i,articleDict[x[0]],x[1])),esa_lines)
            outHandle.write(''.join(el))
            inHandle.close()
        outHandle.close()
