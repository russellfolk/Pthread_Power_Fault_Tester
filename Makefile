# make file...
#
SOURCEDIR = ./src/
OUTPUTDIR = ./bin/
CXX = g++
CC = gcc
CXXFLAGS = -std=c++11 -lpthread -Wall -I $(SOURCEDIR)
CFLAGS = -Wall -c -I $(SOURCEDIR)

MKDIR = mkdir -p $(OUTPUTDIR)
OBJECTC = Fletcher64.o

CXXSOURCES = \
	main.cpp

CSOURCES = \
	Fletcher64.c

all: writer

writer:
	$(MKDIR)
	$(CXX) $(SOURCEDIR)$(CSOURCES) $(CFLAGS) -o $(OUTPUTDIR)$(OBJECTC)
	$(CXX) -o file-writer $(OUTPUTDIR)$(OBJECTC) $(SOURCEDIR)$(CXXSOURCES) $(CXXFLAGS)

.PHONY: clean
clean:
	$(RM) -rf $(OUTPUTDIR)
	$(RM) ./*.gc??
	$(RM) ./*.o
	$(RM) -rf ./*.dSYM
