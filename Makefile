# make file...
#
SRCDIR = ./src/
OUTDIR = ./bin/
OBJDIR = ./obj/
CXX = g++
CXXFLAGS = -std=c++11 -Wall -I $(SRCDIR)
THREADFLAGS = -lpthread
LINKFLAGS = -c

MKDIR = mkdir -p

all: writer checker
	$(RM) -r $(OBJDIR)

writer: $(OBJDIR)writer.o $(OBJDIR)Fletcher64.o $(OBJDIR)file_lib.o
	$(MKDIR) $(OUTDIR)
	$(CXX) $(OBJDIR)writer.o $(OBJDIR)Fletcher64.o $(OBJDIR)file_lib.o -o $(OUTDIR)writer $(CXXFLAGS) $(THREADFLAGS)

checker: $(OBJDIR)checker.o $(OBJDIR)Fletcher64.o $(OBJDIR)file_lib.o
	$(MKDIR) $(OUTDIR)
	$(CXX) $(OBJDIR)checker.o $(OBJDIR)Fletcher64.o $(OBJDIR)file_lib.o -o $(OUTDIR)checker $(CXXFLAGS)

$(OBJDIR)writer.o: $(SRCDIR)writer.cpp $(SRCDIR)writer.h $(SRCDIR)thread_info.h $(SRCDIR)thread_local.h $(SRCDIR)record_indexes.h
	$(MKDIR) $(OBJDIR)
	$(CXX) $(SRCDIR)writer.cpp -o $(OBJDIR)writer.o $(CXXFLAGS) $(LINKFLAGS)

$(OBJDIR)checker.o: $(SRCDIR)checker.cpp $(SRCDIR)checker.h $(SRCDIR)checker_thread_statistics.h $(SRCDIR)record_indexes.h
	$(MKDIR) $(OBJDIR)
	$(CXX) $(SRCDIR)checker.cpp -o $(OBJDIR)checker.o $(CXXFLAGS) $(LINKFLAGS)

$(OBJDIR)Fletcher64.o: $(SRCDIR)Fletcher64.cpp $(SRCDIR)Fletcher64.h
	$(MKDIR) $(OBJDIR)
	$(CXX) $(SRCDIR)Fletcher64.cpp $(LINKFLAGS) $(CXXFLAGS) -o $(OBJDIR)Fletcher64.o

$(OBJDIR)file_lib.o: $(SRCDIR)file_lib.cpp $(SRCDIR)file_lib.h
	$(MKDIR) $(OBJDIR)
	$(CXX) $(SRCDIR)file_lib.cpp $(LINKFLAGS) $(CXXFLAGS) -o $(OBJDIR)file_lib.o

.PHONY: clean
clean:
	$(RM) -r $(OUTDIR)
	$(RM) ./*.gc??
	$(RM) -r ./*.dSYM
