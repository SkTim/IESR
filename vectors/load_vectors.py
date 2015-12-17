#lhy
#2015.8

import cPickle
import os
import word2vec
from multiprocessing import Pool

def loadVector(wordList,index):
    #global wordList
    for word in wordList:
        os.system("java -cp ../../wikiprep-esa/esa-lucene.jar edu.wiki.demo.TestESAVectors %s > ../../wikiprep-esa/vectors/%s" % (word,word))

def loadVectors(wordList,proc_number):
    wordsNum = len(wordList)
    #loadVector(wordList)
    group = int(wordsNum / proc_number)
    start = 0
    p = Pool()
    for i in range(proc_number):
        if i != proc_number - 1:
            p.apply_async(loadVector,args = (wordList[start:start + group],i))
            start += group
        else:
            p.apply_async(loadVector,args = (wordList[start:wordsNum],i))
    p.close()
    p.join()
    print "Vectors Loaded"

if __name__ == "__main__":
    wordList = []
    wordList_wiki = []
    words = cPickle.load(open("../data/wordList_wiki",'rb'))
    words = map((lambda x:x.encode("utf-8")),words)
    print "Words Loaded"
    for word in words:
        if word.isalpha() and '\\' not in str(word):
            wordList_wiki.append(word.lower())
    print "Data Ready"
    #loadVectors(wordList_wiki,20)
    for word in wordList_wiki:
        inHandle = open("../../wikiprep-esa/vectors/%s" % word,'r')
        in_list = inHandle.readlines()
        if len(in_list) > 5:
            wordList.append(word)
    cPickle.dump(wordlist,open("../data/wordList",'wb'))

