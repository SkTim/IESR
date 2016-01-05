#lhy
#2015.2

import cPickle
import PPMI

m = PPMI.Matrix(5,"../data/enwiki.txt")
m.getDict()
print "getDict ok"

m.getIndexDict()
print "getIndexDict ok"

m.getInfoMatrix()
print "getInfoMatrix ok"

m.PPMI()
print "PPMI ok"

m.writeMatrix("../data/infoMatrix")
print "WriteMatrix ok"

#index = m.nmf(400,4)
#print "nmf ok"
