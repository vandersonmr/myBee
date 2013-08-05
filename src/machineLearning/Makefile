CC=g++
CFLAGS=-O3 
dataDAO= -I../include/ 
INCLUDES=-I./ $(dataDAO)

all: limit machineLearning

machineLearning:
	$(CC) $(INCLUDES) $(CFLAGS) $(LIBS) -c -o machineLearning.o machineLearning.cpp 

kcentroid:
	$(CC) $(INCLUDES) $(CFLAGS) $(LIBS) -c -o kcentroid.o kcentroid.cpp

krls:
	$(CC) $(INCLUDES) $(CFLAGS) $(LIBS) -c -o krls_ex.o krls_ex.cpp

limit:
	$(CC)  $(dataDAO) -c -o limit.o limit.cpp

clear:
	rm -f limit.o
	rm -f krls_ex.o
	rm -f kcentroid.o
	rm -f machineLearning.o
