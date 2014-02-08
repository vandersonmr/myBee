CC=clang++
CFLAGS=-std=c++11 -O3
dataDAO= -I../include/
INCLUDES=-I./ $(dataDAO) -g
SRC_MACHINE=$(wildcard *.cpp)
OBJ_MACHINE=$(SRC_MACHINE:.cpp=.o)

all: $(OBJ_MACHINE)

.cpp.o:
	$(CC) -c $< $(CFLAGS) $(INCLUDES) $(dataDAO) -o $@

clear:
	rm -f *.o
