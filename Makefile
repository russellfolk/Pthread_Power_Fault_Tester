# This Makefile will compile all the files inside the src/ directory. It will
# create object files that are stored inside the obj/ directory so that partial
# rebuilds can be performed. Final complete binaries are stored inside the bin/
# directory for running.
#
# These programs require that C++11 be used in compilation.

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

# compile the writer where $@ is writer
writer: $(OBJECTS)
	$(CXX) $(DEPENDENCIES) $(OBJDIR)writer.o -o $(OUTDIR)$@ $(LDFLAGS)

# compile the checker where $@ is checker
checker:$(OBJECTS)
	$(CXX) $(DEPENDENCIES) $(OBJDIR)checker.o -o $(OUTDIR)$@

# compile all cpp files where $< is each of the *.cpp files
# @F implies that only the file portion is used, that is example.cpp = example
.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $(OBJDIR)$(@F)

.PHONY: clean
clean:
	rm -rf $(OUTDIR)
	rm -rf $(OBJDIR)
	@$(RM) ./*.gc??
	@rm -rf ./*.dSYM
