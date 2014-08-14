CC=gcc
CCPP=g++
SRC= ../src
CFLAGS= -O0 -g -Wall -Wextra  -Werror -pedantic-errors -ansi -Winit-self -Wuninitialized -Woverloaded-virtual -Winit-self
SHAREDOBJ= repa.so
LDrepa= -pthread -lpthread -lpython2.7
LDrepaAPI= -lmsgpack --std=c++11 $(LDrepa)
LDmysql=  -lmysqlclient
HEADERS= -I$(SRC)/include/ -I/usr/include/mysql/ -I./repd/
FOLDERS= random arduino raspberry

all: server examples clear.o

server:
	cd $(SRC)/monitorAPI;\
	make;\
	cd -;
	$(CCPP) $(CFLAGS) server.cpp $(SRC)/monitorAPI/libmonitorapi.a $(LDrepaAPI) $(LDmysql) $(HEADERS) -o server
examples:
	$(foreach i, $(FOLDERS), cd $(i); make; cd ..;)

clear.o:
	rm -f *.o
	rm -f *.so

clear:
	$(foreach i, $(FOLDERS), cd $(i); make clear; cd ..;)

