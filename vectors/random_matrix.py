#lhy
#2015.11

import random

inHandle = open(r'../data/infoMatrix_8')
outHandle = open("../data/infoMatrix_8_1",'w')
in_list = inHandle.readlines()

lines = []
matrix_id = 0
outHandle.write(in_list[0])

for i in range(1,len(in_list)):
    if in_list[i] == "PPMI:\n":
        matrix_id = 0
        continue
    if in_list[i] == "ESA:\n":
        matrix_id = 1
        continue
    if matrix_id == 0:
        lines.append("P%s" % in_list[i])
    if matrix_id == 1:
        lines.append("E%s" % in_list[i])
#random.shuffle(lines)

outHandle.write(''.join(lines))
outHandle.close()
