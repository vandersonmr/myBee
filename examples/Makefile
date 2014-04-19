CC=gcc
CCPP=g++
SRC= ../src
CFLAGS= -O0 -g -Wall -Wextra  -Werror -pedantic-errors -ansi -Winit-self -Wuninitialized -Woverloaded-virtual -Winit-self
SHAREDOBJ= repa.so
LDrepa= -pthread -lpthread -lpython2.7
LDrepaAPI= -lmsgpack --std=c++11 $(LDrepa)
LDmysql=  -lmysqlclient
HEADERS= -I$(SRC)/include/ -I/usr/include/mysql/ -I./repd/
FOLDERS= random arduino

all: examples clear.o
	
examples:
	$(foreach i, $(FOLDERS), cd $(i); make; cd ..;)

clear.o:
	rm -f *.o
	rm -f *.so

clear:
	$(foreach i, $(FOLDERS), cd $(i); make clear; cd ..;)

