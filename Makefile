# make file...
#
SOURCEDIR = ./src/
OUTPUTDIR = ./bin/
CXX = g++
CXXFLAGS = -std=c++11 -Wall -I $(SOURCEDIR)
MAINFLAGS = -lpthread
LINKFLAGS = -c

MKDIR = mkdir -p $(OUTPUTDIR)
OBJCKSM = Fletcher64.o
OBJFILLIB = file_lib.o

OUTWRTR = writer
OUTCHKR = checker

all: writer checker

writer:
	$(MKDIR)
	$(CXX) $(SOURCEDIR)Fletcher64.cpp $(LINKFLAGS) $(CXXFLAGS) -o $(OUTPUTDIR)$(OBJCKSM)
	$(CXX) $(SOURCEDIR)file_lib.cpp $(LINKFLAGS) $(CXXFLAGS) -o $(OUTPUTDIR)$(OBJFILLIB)
	$(CXX) -o $(OUTWRTR) $(OUTPUTDIR)$(OBJCKSM) $(OUTPUTDIR)$(OBJFILLIB) $(SOURCEDIR)writer.cpp $(CXXFLAGS) $(MAINFLAGS)

checker:
	$(CXX) -o $(OUTCHKR) $(OUTPUTDIR)$(OBJCKSM) $(OUTPUTDIR)$(OBJFILLIB) $(SOURCEDIR)checker.cpp $(CXXFLAGS)

.PHONY: clean
clean:
	$(RM) -rf $(OUTPUTDIR)
	$(RM) ./*.gc??
	$(RM) ./*.o
	$(RM) -rf ./*.dSYM
	$(RM) $(OUTWRTR) $(OUTCHKR)
