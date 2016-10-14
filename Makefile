# make file...
#
CXX = g++
CC = gcc
CXXFLAGS = -std=c++11 -lpthread -g -Wall
CFLAGS = -g -Wall -c

OUTPUTDIR = ./bin/
MKDIR = mkdir -p $(OUTPUTDIR)
OBJECTC = Fletcher64.o

CXXSOURCES = \
	main.cpp

CSOURCES = \
	Fletcher64.c

all: writer

writer:
	$(MKDIR)
	$(CXX) $(CSOURCES) $(CFLAGS) -o $(OUTPUTDIR)$(OBJECTC)
	$(CXX) -o run $(OUTPUTDIR)$(OBJECTC) $(CXXSOURCES) $(CXXFLAGS)

.PHONY: clean
clean:
	$(RM) -rf $(OUTPUTDIR)
	$(RM) ./*.gc??
	$(RM) ./*.o
