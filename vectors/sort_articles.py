#lhy
#2015.12

inHandle = open(r'../data/infoMatrix_8_1')
in_list = inHandle.readlines()
articleDict = {}
for line in in_list:
    if line[0] != 'E':
        continue
    articleID = line.split(' ')[1]
    if articleID in articleDict:
        articleDict[articleID] += 1
    else:
        articleDict[articleID] = 1
articleList = sorted(articleDict.iteritems(),key = lambda x:x[1], reverse = True)
outHandle = open("../data/articles.txt",'w')
outHandle.write("%d\n" % len(articleDict))
outHandle.write('\n'.join(map((lambda x:"%s %d" % (x[0],x[1])),articleList)))
inHandle.close()
outHandle.close()
