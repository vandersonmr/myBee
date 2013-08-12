CC=g++
CFLAGS=-std=c++11 -O3
dataDAO= -I../include/
INCLUDES=-I./ $(dataDAO) -g

all: limit KCentroid Krls Rvm machineLearning ManipulateData

machineLearning:
	$(CC) $(INCLUDES) $(CFLAGS) $(LIBS) -c -o machineLearning.o machineLearning.cpp

KCentroid:
	$(CC) $(INCLUDES) $(CFLAGS) $(LIBS) -c -o KCentroid.o KCentroid.cpp

Krls:
	$(CC) $(INCLUDES) $(CFLAGS) $(LIBS) -c -o Krls.o Krls.cpp

limit:
	$(CC)  $(dataDAO) -c -o limit.o limit.cpp

Rvm:
	$(CC) $(INCLUDES) $(CFLAGS) $(LIBS) -c -o Rvm.o Rvm.cpp

ManipulateData:
	$(CC) $(INCLUDES) $(CFLAGS) $(LIBS) -c -o ManipulateData.o ManipulateData.cpp

clear:
	rm -f *.o
