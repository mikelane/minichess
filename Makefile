CC = g++
DEBUG = -Wall -g
CPPFLAGS = -Wall
C11 = --std=c++11
OFLAGS = -O4
LIBS = -lzmq
BINDIR = source/move_generator/bin/
SRCDIR = source/move_generator/

SOURCES = $(SRCDIR)*.cpp
OFILE = -o $(BINDIR)move_generator

# g++ --std=c++11 -o source/move_generator/bin/move_generator source/move_generator/*.cpp -g -lzmq -Wall

debug:
	$(CC) $(C11) $(OFILE) $(SOURCES) $(DEBUG) $(LIBS)

odebug:
	$(CC) $(C11) $(OFILE) $(SOURCES) $(DEBUG) $(LIBS) $(OFLAGS)

optimized:
	$(CC) $(C11) $(OFILE) $(SOURCES) $(LIBS) $(OFLAGS)

standard:
	$(CC) $(C11) $(OFILE) $(SOURCES) $(LIBS)
