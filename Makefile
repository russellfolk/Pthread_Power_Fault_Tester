# make file...
#
SRCDIR       = $(wildcard src/*.cpp)
OUTDIR       = bin/
OBJDIR       = obj/
OBJECTS      = $(SRCDIR:.cpp=.o)
DEPENDENCIES = $(OBJDIR)Fletcher64.o $(OBJDIR)file_lib.o
CXX          = g++
CXXFLAGS     = -c -std=c++11 -Wall
LDFLAGS      = -lpthread
MKDIR        = mkdir -p

all: paths writer checker

.PHONY: paths
paths:
	$(MKDIR) $(OBJDIR)
	$(MKDIR) $(OUTDIR)

writer: $(OBJECTS)
	$(CXX) $(DEPENDENCIES) $(OBJDIR)writer.o -o $(OUTDIR)$@ $(LDFLAGS)

checker:$(OBJECTS)
	$(CXX) $(DEPENDENCIES) $(OBJDIR)checker.o -o $(OUTDIR)$@

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $(OBJDIR)$(@F)

.PHONY: clean
clean:
	$(RM) -r $(OUTDIR)
	$(RM) ./*.gc??
	$(RM) -r ./*.dSYM
