# Source file locations, output, compiler
TESTFILES = main.cpp tests.cpp
CODEFILE = ../MidiOsc/MidiOsc/main.c
CODEHEADER = test.h
OUT = test
CC = cc
CXX = c++

# CppUTest directory
CPPUTEST_HOME = /usr/local/Cellar/cpputest/3.8

CFLAGS = -Wall -g# -g = generate complete debug info
CXXFLAGS = -Wall -g

CPPFLAGS += -I$(CPPUTEST_HOME)/include# -I adds directories to search for include files
CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt# -L adds directory to library search path

build: comp_code comp_tests
	$(CXX) $(CXXFLAGS) $(LD_LIBRARIES) -o $(OUT) code.o tests.o main.o

clean_all: clean object_clean

clean:
	-rm -f $(OUT)

object_clean:
	-rm -f *.o

rebuild: clean build

comp_code: $(CODEFILE) $(CODEHEADER)
	$(CC) -D TEST $(CFLAGS) -c $(CODEFILE) -o code.o
	# -D TEST defines macro "TEST" during compilation

comp_tests: comp_main comp_test

comp_main: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

comp_test: tests.cpp $(CODEHEADER)
	$(CXX) $(CXXFLAGS) -c tests.cpp -o tests.o
	
release: $(CODEFILE)
	$(CC) $(CFLAGS) -o MidiOsc $(CODEFILE)