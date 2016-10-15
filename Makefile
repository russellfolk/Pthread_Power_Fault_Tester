# make file...
#
SRCDIR   = src/
OUTDIR   = bin/
OBJDIR   = obj/
OBJECTS  = $(SRCDIR:.cpp=.o)
CXX      = g++
CXXFLAGS = -c -std=c++11 -Wall -I $(SRCDIR)
LDFLAGS  = -lpthread

MKDIR = mkdir -p

all: writer checker
	$(RM) -r $(OBJDIR)

# $@ == file."output"
# $< == file."input"
.cpp.o: $(OBJECTDIR)
	$(CXX) $(CXXFLAGS) $(SRCDIR)$< -o $(OBJDIR)$@

$(OBJECTDIR):
	$(MKDIR) $(OUTDIR)

writer: $(OBJECTS)
	$(CXX) $(OBJDIR)writer.o $(OBJDIR)Fletcher64.o $(OBJDIR)file_lib.o -o $(OUTDIR)writer $(LDFLAGS)
checker: $(OBJECTS)
	$(CXX) $(OBJDIR)checker.o $(OBJDIR)Fletcher64.o $(OBJDIR)file_lib.o -o $(OUTDIR)checker

.PHONY: clean
clean:
	$(RM) -r $(OUTDIR)
	$(RM) ./*.gc??
	$(RM) -r ./*.dSYM
