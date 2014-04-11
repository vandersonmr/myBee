ARCH=$(shell uname -m)
CC=gcc
CCPP=g++
SRC= ./src
CFLAGS= -O0 -g -Wall -Wextra  -Werror -pedantic-errors -ansi -Winit-self -Wuninitialized -Woverloaded-virtual -Winit-self
SHAREDOBJ= repa.so
LDrepa= -pthread -lpthread -lpython2.7
LDrepaAPI= -lmsgpack --std=c++11 $(LDrepa)
LDmysql=  -lmysqlclient
HEADERS= -I$(SRC)/include/ -I/usr/include/mysql/ -I./repd/

all: server client clear.o
	
server: monitorAPI.o
	$(CCPP) $(CFLAGS) $(SRC)/server.cpp $(SRC)/monitorAPI/libmonitorapi.a $(LDrepaAPI) $(LDmysql) $(HEADERS) -o server

client: monitorAPI.o 
	$(CCPP) $(CFLAGS) $(SRC)/client.cpp $(SRC)/monitorAPI/libmonitorapi.a -o client $(CFLAGS) $(LDrepaAPI) $(HEADERS) -lm

init:
	@echo "Iniciando repd..."
	@sudo ./repd/repad_$(ARCH);\
	if [ $$? -eq 0 ]; then\
		echo "Sucesso.";\
		else echo "Falha.";\
		fi;

kill:
	@echo "Fechando programa..."
	@ps -e | grep repad_$(ARCH) | sudo -s kill $$(awk '{print $$1}');\
	if [ $$? -eq 0 ]; then\
		echo "Programa fechado.";\
		else echo "Programa inexistente";\
		fi;

monitorAPI.o:
	cd ./$(SRC)/monitorAPI;\
	make;\
	cd -;

clear.o:
	rm -f *.o
	rm -f *.so

clear: clear.o
	rm -f client;\
	rm -f server;\
	rm -f $(SRC)/web-UI/getDados;
	cd ./$(SRC)/monitorAPI/machineLearning;\
	make clear;\
	cd -;
	cd ./$(SRC)/monitorAPI;\
	make clear;\
	cd -;
