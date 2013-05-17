arch=x86_64
cc=gcc
flags=-O3 -w -Irepa/ repa/build/lib.linux-$(arch)-2.7/repa.so -lpthread -lpython2.7
all: repad servidor client
repad:
	cd ./repa;\
	python setup.py build;\
	cd ..;
servidor: repad
	$(cc) servidor2.c $(flags) -I/usr/include/mysql -lmysqlclient -o servidor 
client: repad 
	$(cc) $(flags) client2.c -o  client



