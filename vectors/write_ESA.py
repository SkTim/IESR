#lhy
#2015.12

import cPickle

#Write word vectors in current dist
def write_vector(word):
    try:
        inHandle = open("/home/lhy/vectors/%s" % word,'r')
    except:
        print word
        return 1
    vectors = inHandle.read()
    outHandle = open("../../wikiprep-esa/vectors/%s" % word,'w')
    outHandle.write(vectors)
    inHandle.close()
    outHandle.close()
    return 0

if __name__ == "__main__":
    wordList = cPickle.load(open("../data/wordList_test",'rb'))
    l = map(write_vector,wordList)
    print sum(l)
