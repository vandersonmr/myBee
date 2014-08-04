CC=g++
CFLAGS=-std=c++11 -O0 -g -Wall -Wextra -Werror -pedantic-errors -Winit-self -Wmissing-declarations -Wuninitialized -Woverloaded-virtual -Winit-self
dataDAO= -I../include/
INCLUDES=-I./ $(dataDAO) -g
SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)

all: $(OBJ)

.cpp.o:
	$(CC) -c $< $(CFLAGS) $(INCLUDES) -o $@

clear:
	rm -f *.o
