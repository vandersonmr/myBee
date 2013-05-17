arch=x86_64
cc=gcc
flags=-O3 -w -Irepa/ repa/build/lib.linux-$(arch)-2.7/repa.so -lpthread -lpython2.7
all: repad servidor client
repad:
	cd ./repa;\
	python setup.py build;\
	cd ..;
servidor: repad
	$(cc) servidor2.c -I/usr/include/mysql -lmysqlclient -o servidor $(flags)
client: repad 
	$(cc) client2.c -o  client $(flags)



