# make file...
#
CC=g++
CFLAGS=-std=c++11 -pthread -g -Wall

all: writer

writer:
	$(CC) $(CFLAGS) -o run main.cpp

clean:
	$(RM) run
