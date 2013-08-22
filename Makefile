ARCH=$(shell uname -p)
CC=gcc
CCPP=g++
SRC=./src
CFLAGS=-O0 -g -w
SHAREDOBJ= ./radnet-desktop-$(ARCH)/libs/librepa_$(ARCH).so
LDrepa= -pthread -lpthread $(SHAREDOBJ) -lpython2.7
LDmysql=  -lmysqlclient 
HEADERS= -I$(SRC)/include/ -I/usr/include/mysql/ -I./repd/

all: servidor client cgi clear.o

dataDAO.o:
	$(CCPP) -c $(SRC)/database/dataDAO.c -o dataDAO.o $(CFLAGS) $(HEADERS) -fpermissive 

servidor.o: 
	$(CCPP) -c $(SRC)/servidor2.c -o servidor.o $(CFLAGS) $(HEADERS)

servidor: dataDAO.o servidor.o machineLearning.o
	$(CCPP) servidor.o dataDAO.o $(SRC)/machineLearning/*.o $(LDrepa) $(LDmysql) $(HEADERS) -o servidor 
	
client:
	$(CC) $(SRC)/client2.c -o client $(CFLAGS) $(LDrepa) $(HEADERS) -lm

cgi:
	$(CCPP) $(SRC)/web-UI/CGI/getDados.c $(SRC)/database/dataDAO.c $(LDmysql) -o $(SRC)/web-UI/getDados $(CFLAGS) $(HEADERS) -fpermissive 

install-lib:
	sudo cp $(SHAREDOBJ) /usr/local/lib
	sudo ldconfig

install:
	sudo mkdir -p /var/www/lang
	sudo cp ./$(SRC)/web-UI/*.html /var/www/
	sudo cp ./$(SRC)/web-UI/*.css /var/www/
	sudo cp ./$(SRC)/web-UI/*.js /var/www/	
	sudo cp ./$(SRC)/web-UI/getDados /usr/lib/cgi-bin/
	sudo cp ./$(SRC)/web-UI/lang/* /var/www/lang/
	sudo mkdir -p /usr/lib/cgi-bin/config/
	sudo cp ./config/db.conf /usr/lib/cgi-bin/config/ 
	sudo chmod 777 /usr/lib/cgi-bin/getDados
	
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

clear.o:
	rm -f *.o 
clear: clear.o
	rm -f client;\
	rm -f servidor;\
	rm -f $(SRC)/web-UI/getDados;
	cd ./$(SRC)/machineLearning;\
	make clear;\
	cd -;
