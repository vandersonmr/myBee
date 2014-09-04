CC=gcc
CCPP=g++
SRC= ../src
CFLAGS= -O0 -g -Wall -Wextra  -Werror -pedantic-errors -ansi -Winit-self -Wuninitialized -Woverloaded-virtual -Winit-self
SHAREDOBJ= repa.so
LDrepa= -pthread -lpthread -lpython2.7
LDrepaAPI= -lmsgpack --std=c++11 $(LDrepa)
LDmysql=  -lmysqlclient
HEADERS= -I$(SRC) -I/usr/include/mysql/ -I./repd/
FOLDERS= random arduino raspberry

all: server examples clear.o

server: monitorAPI.o
	$(CCPP) $(CFLAGS) server.cpp $(SRC)/monitorAPI/libmonitorapi.a $(LDrepaAPI) $(LDmysql) $(HEADERS) -o server

monitorAPI.o:
	cd $(SRC)/monitorAPI;\
	make;\
	cd -;

examples:
	$(foreach i, $(FOLDERS), cd $(i); make; cd ..;)

clear.o:
	rm -f *.o
	rm -f *.so
	cd ..;\
	make clear;

clear:
	rm -f server
	$(foreach i, $(FOLDERS), cd $(i); make clear; cd ..;)

