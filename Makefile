ARCH=$(shell uname -p)
CC=gcc
CFLAGS=-O0 -g -w
LDrepa= -Irepa/ -lpthread repa/build/lib.linux-$(ARCH)-2.7/repa.so -lpython2.7
LDmysql= -I/usr/include/mysql/ -lmysqlclient 
HEADERS= -Iinclude/

all: repad servidor client cgi

repad:
	cd ./repa;\
	python setup.py build;\
	cd ..;

servidor: repad
	$(CC) servidor2.c database/dataDAO.c $(LDrepa) $(LDmysql) -o servidor $(CFLAGS) $(HEADERS)

client: repad 
	$(CC) client2.c -o  client $(CFLAGS) $(LDrepa)

cgi:
	$(CC) web-UI/CGI/getDados.c database/dataDAO.c $(LDmysql) -o web-UI/getDados $(CFLAGS) $(HEADERS) 

install:
	sudo cp ./web-UI/*.html /var/www/
	sudo cp ./web-UI/*.css /var/www/
	sudo cp ./web-UI/*.js /var/www/	
	sudo cp ./web-UI/getDados /usr/lib/cgi-bin/ 
	sudo chmod 777 /usr/lib/cgi-bin/getDados

clear:
	rm -f client;\
	rm -f servidor;\
	rm -f web-UI/getDados;
