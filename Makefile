# make file...
#
SRCDIR = src/
OUTDIR = bin/
OBJDIR = obj/
CXX = g++
CXXFLAGS = -c -std=c++11 -Wall -I $(SRCDIR)
LDFLAGS = -lpthread

MKDIR = mkdir -p

all: writer checker

writer: $(OBJDIR)writer.o $(OBJDIR)Fletcher64.o $(OBJDIR)file_lib.o
	@$(MKDIR) $(OUTDIR)
	$(CXX) $(OBJDIR)writer.o $(OBJDIR)Fletcher64.o $(OBJDIR)file_lib.o -o $(OUTDIR)writer $(LDFLAGS)

checker: $(OBJDIR)checker.o $(OBJDIR)Fletcher64.o $(OBJDIR)file_lib.o
	@$(MKDIR) $(OUTDIR)
	$(CXX) $(OBJDIR)checker.o $(OBJDIR)Fletcher64.o $(OBJDIR)file_lib.o -o $(OUTDIR)checker

$(OBJDIR)writer.o: $(SRCDIR)writer.cpp $(SRCDIR)writer.h $(SRCDIR)thread_info.h $(SRCDIR)thread_local.h $(SRCDIR)record_indexes.h
	@$(MKDIR) $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(SRCDIR)writer.cpp -o $(OBJDIR)writer.o

$(OBJDIR)checker.o: $(SRCDIR)checker.cpp $(SRCDIR)checker.h $(SRCDIR)checker_thread_statistics.h $(SRCDIR)record_indexes.h
	@$(MKDIR) $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(SRCDIR)checker.cpp -o $(OBJDIR)checker.o

$(OBJDIR)Fletcher64.o: $(SRCDIR)Fletcher64.cpp $(SRCDIR)Fletcher64.h
	@$(MKDIR) $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(SRCDIR)Fletcher64.cpp -o $(OBJDIR)Fletcher64.o

$(OBJDIR)file_lib.o: $(SRCDIR)file_lib.cpp $(SRCDIR)file_lib.h
	@$(MKDIR) $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(SRCDIR)file_lib.cpp -o $(OBJDIR)file_lib.o

.PHONY: clean
clean:
	rm -rf $(OBJDIR)
	rm -rf $(OUTDIR)
	@$(RM) ./*.gc??
	@rm -rf ./*.dSYM
