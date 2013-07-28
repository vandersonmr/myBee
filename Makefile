arch=x86_64
cc=gcc
flags=-O0 -g -w -Irepa/ repa/build/lib.linux-$(arch)-2.7/repa.so -lpthread -lpython2.7 
all: repad servidor client cgi
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
	sudo cp web-UI/*.html /var/www/ -f
	sudo cp web-UI/*.js /var/www/ -f
	sudo cp web-UI/getDados /usr/lib/cgi-bin/ -f

clear:
	rm -f client;\
	rm -f servidor;\
	rm -f web-UI/getDados;
	



