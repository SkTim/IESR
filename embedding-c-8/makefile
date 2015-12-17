BIN_DIR=../bin

CC=gcc
CFLAGS=-lm -pthread -O3 -Wall -march=native -funroll-loops  -Wno-unused-result -I/usr/include/malloc # OS X

all: embedding_8

embedding_8 : embedding_8.c
	$(CC) embedding_8.c -o ${BIN_DIR}/embedding_8 $(CFLAGS)
