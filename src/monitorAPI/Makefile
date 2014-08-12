CC=g++
CFLAGS=-std=c++11 -O0 -g -Wall -Wextra -Werror -pedantic-errors -Winit-self -Wmissing-declarations -Wuninitialized -Woverloaded-virtual -Winit-self
dataDAO= -I../include/
INCLUDES=-I./ $(dataDAO) -g
SRC_MACHINE=$(wildcard *.cpp)
OBJ_MACHINE=$(SRC_MACHINE:.cpp=.o)

all: libmonitorapi.a

libmonitorapi.a: db	ml repa.so $(OBJ_MACHINE)
	ar -cvq libmonitorapi.a *.o database/*.o machineLearning/*.o repa.so

.cpp.o: 
	$(CC) -c $< $(CFLAGS) $(INCLUDES) $(dataDAO) -o $@

apiClient: repa.so
	$(CC) -c client_monitor.cpp $(CFLAGS) $(INCLUDES)
	ar -cvq libmonitorapi.a *.o repa.so

repa.so:
	cd ./repa/ &&\
	python setup.py build &&\
	sudo python setup.py install &&\
	cd ./build/ &&\
	cd $$(ls | egrep '^lib') &&\
	cp repa.so ../../../ &&\
	sudo cp repa.so /usr/lib/ &&\
	sudo ldconfig

ml: 
	cd machineLearning; \
	make;

db:
	cd database; \
	make;

clear:
	cd database; \
	make clear;
	cd machineLearning; \
	make clear;
	rm -f *.a
	rm -f *.so
	rm -f *.o
