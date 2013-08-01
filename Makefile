arch=i686
cc=gcc
flags=-O0 -g -w -Irepa/ repa/build/lib.linux-$(arch)-2.7/repa.so -lpthread -lpython2.7 
all: repad servidor client
repad:
	cd ./repa;\
	python setup.py build;\
	cd ..;
servidor: repad
	$(cc) servidor2.c database/dataDAO.c -I/usr/include/mysql -lmysqlclient -o servidor $(flags) -Iinclude/
client: repad 
	$(cc) client2.c -o  client $(flags)
cgi:
	$(cc) web-UI/CGI/getDados.c database/dataDAO.c -o web-UI/getDados $(flags) -Iinclude/ -I/usr/include/mysql -lmysqlclient
	sudo cp web-UI/*.html /var/www/
	sudo cp web-UI/*.js /var/www/
	sudo cp web-UI/*.css /var/www/
	sudo cp web-UI/getDados /usr/lib/cgi-bin/ 
	sudo chmod 777 /usr/lib/cgi-bin/getDados
clear:
	rm -f client;\
	rm -f servidor;\
	rm -f web-UI/getDados;
