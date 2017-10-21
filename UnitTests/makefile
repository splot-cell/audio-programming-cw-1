# Source file locations, output, compiler
TESTFILES = main.cpp tests.cpp
CODEFILE = ../MidiOsc/MidiOsc/main.c
TESTHEADER = test.h
OUT = test
CC = cc
CXX = c++

# CppUTest directory
CPPUTEST_HOME = /usr/local/Cellar/cpputest/3.8

CFLAGS = -Wall -g -pedantic# -g = generate complete debug info
CXXFLAGS = -Wall -g -pedantic

CPPFLAGS += -I$(CPPUTEST_HOME)/include# -I adds directories to search for include files
CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt# -L adds directory to library search path

tests: comp_code comp_tests
	$(CXX) $(CXXFLAGS) $(LD_LIBRARIES) -o $(OUT) code.o tests.o main.o

clean_all: clean object_clean

clean:
	-rm -f $(OUT)

object_clean:
	-rm -f *.o

rebuild_tests: clean tests

comp_code: $(CODEFILE) $(TESTHEADER)
	$(CC) -D TEST $(CFLAGS) -c $(CODEFILE) -o code.o
# -D TEST defines macro "TEST" during compilation

comp_tests: comp_main comp_test

comp_main: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

comp_test: tests.cpp $(CODEFILE) $(TESTHEADER)
	$(CXX) $(CXXFLAGS) -c tests.cpp -o tests.o
	
release: $(CODEFILE)
	$(CC) $(CFLAGS) -o MidiOsc $(CODEFILE)