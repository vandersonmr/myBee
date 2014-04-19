ARCH=$(shell uname -m)
CC=gcc
CCPP=g++
SRC= ../../src
CFLAGS= -O0 -g -Wall -Wextra -Werror -pedantic-errors -ansi -Winit-self -Wuninitialized -Woverloaded-virtual -Winit-self
SHAREDOBJ= repa.so
LDrepa= -pthread -lpthread -lpython2.7
LDrepaAPI= -lmsgpack --std=c++11 $(LDrepa)
LDmysql= -lmysqlclient
HEADERS= -I$(SRC)/include/ -I$(SRC) -I../../repd/ 

all: server client clear.o

server: monitorAPI.o
	$(CCPP) $(CFLAGS) server.cpp $(SRC)/monitorAPI/libmonitorapi.a $(LDrepaAPI) $(LDmysql) $(HEADERS) -o server

client: monitorAPI.o
	$(CCPP) $(CFLAGS) client.cpp $(SRC)/monitorAPI/libmonitorapi.a -o client $(CFLAGS) $(LDrepaAPI) $(HEADERS) -lm

monitorAPI.o:
		cd ./$(SRC)/monitorAPI;\
		make;\
		cd -;

clear.o:
	rm -f *.o
	rm -f *.so

clear:
	rm client
	rm server

