ARCH=$(shell uname -m)
CC=gcc
CCPP=g++
SRC= ./src
CFLAGS= -O0 -g -Wall -Wextra  -Werror
SHAREDOBJ= repa.so
LDrepa= -pthread -lpthread $(SHAREDOBJ) -lpython2.7
LDrepaAPI= -lmsgpack --std=c++11 $(LDrepa)
LDmysql=  -lmysqlclient
HEADERS= -I$(SRC)/include/ -I/usr/include/mysql/ -I./repd/

all: repa server client clear.o
	
server: machineLearning.o repa monitorAPI.o
	$(CCPP) $(CFLAGS) $(SRC)/server.cpp $(SRC)/monitorAPI/server_monitor.o $(SRC)/database/dataDAO.cpp  $(SRC)/machineLearning/*.o $(LDrepaAPI) $(LDmysql) $(HEADERS) -o server

client: repa monitorAPI.o 
	$(CCPP) $(CFLAGS) $(SRC)/client.cpp $(SRC)/monitorAPI/client_monitor.o -o client $(CFLAGS) $(LDrepaAPI) $(HEADERS) -lm

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

machineLearning.o:
	cd ./$(SRC)/machineLearning;\
	make;\
	cd -;

monitorAPI.o:
	cd ./$(SRC)/monitorAPI;\
	make;\
	cd -;

repa:
	cd ./src/repa/ &&\
	python setup.py build &&\
	sudo python setup.py install &&\
	cd ./build/ &&\
	cd $$(ls | egrep '^lib') &&\
	cp repa.so ../../../../ &&\
	sudo cp repa.so /usr/lib/ &&\
	sudo ldconfig

clear.o:
	rm -f *.o
	rm -f *.so

clear: clear.o
	rm -f client;\
	rm -f server;\
	rm -f $(SRC)/web-UI/getDados;
	cd ./$(SRC)/machineLearning;\
	make clear;\
	cd -;
	cd ./$(SRC)/monitorAPI;\
	make clear;\
	cd -;
