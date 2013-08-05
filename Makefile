ARCH=$(shell uname -p)
CC=gcc
CCPP=g++
SRC=./src
CFLAGS=-O0 -g -w
LDrepa= -lpthread $(SRC)/repa/build/lib.linux-$(ARCH)-2.7/repa.so -lpython2.7
LDmysql=  -lmysqlclient 
HEADERS= -I$(SRC)/include/ -I/usr/include/mysql/ -I$(SRC)/repa/

all: repad servidor client cgi clear.o

repad:
	cd ./$(SRC)/repa;\
	python setup.py build;\
	cd ../..;

dataDAO.o:
	$(CCPP) -c $(SRC)/database/dataDAO.c -o dataDAO.o $(CFLAGS) $(HEADERS) -fpermissive 

servidor.o: 
	$(CCPP) -c $(SRC)/servidor2.c -o servidor.o $(CFLAGS) $(HEADERS)  

servidor: repad dataDAO.o servidor.o machineLearning.o
	$(CCPP) servidor.o dataDAO.o $(SRC)/machineLearning/machineLearning.o $(LDrepa) $(LDmysql) $(HEADERS) -o servidor 
	
client: repad 
	$(CC) $(SRC)/client2.c -o  client $(HEADERS) $(CFLAGS) $(LDrepa) -lm

cgi:
	$(CCPP) $(SRC)/web-UI/CGI/getDados.c $(SRC)/database/dataDAO.c $(LDmysql) -o $(SRC)/web-UI/getDados $(CFLAGS) $(HEADERS) -fpermissive 

install:
	sudo cp ./$(SRC)/web-UI/*.html /var/www/
	sudo cp ./$(SRC)/web-UI/*.css /var/www/
	sudo cp ./$(SRC)/web-UI/*.js /var/www/	
	sudo cp ./$(SRC)/web-UI/getDados /usr/lib/cgi-bin/
	sudo mkdir -p /usr/lib/cgi-bin/config/
	sudo cp ./config/db.conf /usr/lib/cgi-bin/config/ 
	sudo chmod 777 /usr/lib/cgi-bin/getDados

machineLearning.o:
	cd ./$(SRC)/machineLearning;\
	make;\
	cd -;

clear.o:
	rm -f *.o 
clear: clear.o
	rm -f client;\
	rm -f servidor;\
	rm -f $(SRC)/web-UI/getDados;
	cd ./$(SRC)/machineLearning;\
	make clear;\
	cd -;
