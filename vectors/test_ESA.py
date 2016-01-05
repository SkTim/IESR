#lhy
#2016.1

import sys
import word2vec
import numpy as np
from make_ESA import get_wordsim

def optimize(a,b):
    if a > b:
        return a
    return 0

def get_line(word,vector):
    #Get a line of word vectors file
    word_vector = [word]
    l = max(vector)
    line = map((lambda x:str(optimize(x,l * 0.7))),list(vector))
    word_vector.extend(line)
    return ' '.join(word_vector)

if __name__ == "__main__":
    vocab_model = word2vec.load("../data/text8.txt")
    article_model = word2vec.load("../data/article8.txt")
    wordSim = get_wordsim(sys.argv[1])
    wordList = []
    vocab_vectors = []
    wordSet = set(list(vocab_model.vocab))
    for word in wordSim:
        if word not in wordSet:
            continue
        wordList.append(word)
        vocab_vectors.append(list(vocab_model[word]))
    print "WordSim: %d, WordList: %d" % (len(wordSim),len(wordList))
    vocab_vectors = np.array(vocab_vectors, dtype = np.float32)
    print "Data Loaded"
    article_vectors = np.array(map((lambda x:list(article_model[x])),list(article_model.vocab)))
    vocab_vectors = vocab_vectors.dot(article_vectors.T)
    word_vectors = map(get_line,wordList,list(vocab_vectors))
    outHandle = open("../data/text8_ESA_%s.txt" % sys.argv[1],'w')
    outHandle.write("%d 171332\n" % len(wordList))
    print "WordVectors ok"
    outHandle.write('\n'.join(word_vectors))
    outHandle.close()
