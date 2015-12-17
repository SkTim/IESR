BIN_DIR=../bin

CC=gcc
CFLAGS=-lm -pthread -O3 -Wall -march=native -funroll-loops  -Wno-unused-result -I/usr/include/malloc # OS X

all: embedding_p

embedding_p : embedding_p.c
	$(CC) embedding_p.c -o ${BIN_DIR}/embedding_p $(CFLAGS)
