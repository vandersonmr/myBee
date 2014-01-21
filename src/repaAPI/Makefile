all: repaAPI

repaAPI:
	clang++ -shared -o librepaAPI.so -fPIC repaAPI.cpp -std=c++11 -lmsgpack -pthread -lpthread -lpython2.7 ../../repa.so

clear: 
	rm -f librepaAPI.so 
