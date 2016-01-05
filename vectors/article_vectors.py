#lhy
#2015.11

import cPickle

inHandle1 = open(r'../../wikiprep-esa/selected.txt')
in_list1 = inHandle1.readlines()
articleList = map((lambda x:x.strip('\n')),in_list1)
inHandle2 = open(r'../data/articleVectors_8')
in_list2 = inHandle2.readlines()
outHandle = open("../data/article8.txt",'w')
for (i,line) in enumerate(in_list2):
    if i == 0:
        outHandle.write(in_list2[i])
        continue
    outHandle.write("%s %s" % (articleList[i - 1],in_list2[i]))
outHandle.close()
