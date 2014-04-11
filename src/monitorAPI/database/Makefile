CC=g++
CFLAGS=-std=c++11 -O0 -g -Wall -Wextra -Werror -pedantic-errors -Winit-self -Wmissing-declarations -Wuninitialized -Woverloaded-virtual -Winit-self
dataDAO= -I../include/
INCLUDES=-I./ $(dataDAO) -g

all:
	g++ -c dataDAO.cpp $(CFLAGS) $(INCLUDES)
clear:
	rm -f *.o
