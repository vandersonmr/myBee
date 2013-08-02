CC=g++
CFLAGS=-O0
dataDAO= -I../include/
INCLUDES=-I./ $(dataDAO) ./dlib/all/source.cpp
LIBS= -lpthread -lX11

all: krls kcentroid limit

kcentroid:
	$(CC) $(INCLUDES) $(CFLAGS) $(LIBS) kcentroid.cpp -o kcentroid
krls:
	$(CC) $(INCLUDES) $(CFLAGS) $(LIBS) krls_ex.cpp -o krls_ex 
limit:
	$(CC)  $(dataDAO) limit.cpp -o limit 

clear:
	rm -f limit
	rm -f krls_ex
	rm -f kcentroid
