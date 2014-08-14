ARCH=$(shell uname -m)
CC=gcc
CCPP=g++
SRC= ../../src
CFLAGS= -O0 -g -Wall -Wextra -Werror -pedantic-errors -ansi -Winit-self -Wuninitialized -Woverloaded-virtual -Winit-self
SHAREDOBJ= repa.so
LDrepa= -pthread -lpthread -lpython2.7
LDrepaAPI= -lmsgpack --std=c++11 $(LDrepa) -lwiringPi
LDmysql= -lmysqlclient
HEADERS= -I$(SRC)/include/ -I$(SRC) -I../../repd/ 

all: monitorClient client clear.o

client:
	$(CCPP) $(CFLAGS) client.cpp $(SRC)/monitorAPI/libmonitorapi.a -o client $(CFLAGS) $(LDrepaAPI) $(HEADERS) -lm

monitorClient:
	cd ./$(SRC)/monitorAPI;\
	make apiClient;\

monitorAPI.o:
		cd ./$(SRC)/monitorAPI;\
		make;\
		cd -;

clear.o:
	rm -f *.o
	rm -f *.so

clear:
	rm -f client
	rm -f server

