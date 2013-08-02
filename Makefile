ARCH=$(shell uname -p)
CC=gcc
SRC=./src
CFLAGS=-O0 -g -w
LDrepa= -I$(SRC)/repa/ -lpthread $(SRC)/repa/build/lib.linux-$(ARCH)-2.7/repa.so -lpython2.7
LDmysql= -I/usr/include/mysql/ -lmysqlclient 
HEADERS= -I$(SRC)/include/

all: repad servidor client cgi

repad:
	cd ./$(SRC)/repa;\
	python setup.py build;\
	cd ../..;

servidor: repad
	$(CC) $(SRC)/servidor2.c $(SRC)/database/dataDAO.c $(LDrepa) $(LDmysql) -o servidor $(CFLAGS) $(HEADERS)

client: repad 
	$(CC) $(SRC)/client2.c -o  client $(CFLAGS) $(LDrepa)

cgi:
	$(CC) $(SRC)/web-UI/CGI/getDados.c $(SRC)/database/dataDAO.c $(LDmysql) -o $(SRC)/web-UI/getDados $(CFLAGS) $(HEADERS) 

install:
	sudo cp ./$(SRC)/web-UI/*.html /var/www/
	sudo cp ./$(SRC)/web-UI/*.css /var/www/
	sudo cp ./$(SRC)/web-UI/*.js /var/www/	
	sudo cp ./$(SRC)/web-UI/getDados /usr/lib/cgi-bin/ 
	sudo chmod 777 /usr/lib/cgi-bin/getDados

clear:
	rm -f client;\
	rm -f servidor;\
	rm -f $(SRC)/web-UI/getDados;
