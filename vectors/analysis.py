#lhy
#2015.4

import cPickle
import random

class Analysis():
    def __init__(self,vectors):
        self.vectors = vectors
        self.words = self.vectors.keys()
        self.scale = len(self.vectors[self.words[0]])
        self.dimensions = {}

    def dimension_analysis(self):
        self.topWords = {}
        for i in range(self.scale):
            self.dimensions[i] = {}
        for i in range(len(self.words)):
            vector = list(self.vectors[self.words[i]])
            for j in range(len(vector)):
                if vector[j] > 0:
                    self.dimensions[j][self.words[i]] = vector[j]
        for i in range(self.scale):
            self.topWords[i] = sorted(self.dimensions[i].iteritems(),key = lambda x:x[1],reverse = True)[0:5]
            #dictionary = {}
            #for (key,value) in self.dimensions[i].items():
            #    if value not in dictionary:
            #        dictionary[value] = [key]
            #    else:
            #        dictionary[value].append(key)
            #index = dictionary.keys()
            #index.sort()
            #for j in range(len(index)):
            #    self.topWords[i].extend(dictionary[index[j]])
        print "Top Words ok"
        outHandle = open("top_words/words",'w')
        for i in range(len(self.topWords)):
            words = [index[0] for index in self.topWords[i]]
            #d = len(self.topWords[i])
            #words = self.topWords[i][d - 5:d]
            #location = int(5 * random.random())
            #d = 0
            #legal_d = 0
            #while legal_d == 0:
            #    d = int(random.random() * 400)
            #    if d in self.topWords:
            #        legal_d = 1
            #words.insert(location,self.topWords[d][0][0])
            #words.append(str(location))
            string = ' '.join(words)
            outHandle.write(string.encode("utf-8"))
            outHandle.write('\n')
        outHandle.close()
        print "Data Ready"
