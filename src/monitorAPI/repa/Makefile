all: repa.so
repa.so:
		python setup.py build &&\
		sudo python setup.py install &&\
		cd ./build/ &&\
		cd $$(ls | egrep '^lib') &&\
		cp repa.so ../../librepa.so &&\
		sudo cp repa.so /usr/lib/ &&\
		sudo ldconfig
