#lhy
#2015.12

import os
import cPickle

wordList = []
for root, dir, filenames in os.walk("../../vectors/"):
    for fn in filenames:
        inHandle = open("../../vectors/%s" % fn)
        if len(inHandle.readlines()) > 0:
            wordList.append(fn)

print len(wordList)
cPickle.dump(wordList,open("wordList_test",'wb'))
