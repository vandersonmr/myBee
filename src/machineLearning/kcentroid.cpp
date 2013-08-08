/*
	The kcentroid object is an implementation of an algorithm that recursively
    	computes the centroid (i.e. average) of a set of points. 

	For another example: http://dlib.net/kcentroid_ex.cpp.html
*/

#include <iostream>
#include <vector>
#include <stdio.h>

#include <dlib/svm.h>
#include <dlib/statistics.h>
#include <dataDAO.h>

using namespace std;
using namespace dlib;

#define limit 0.92
#define sizeOfKnow 1000
#define ACCURACY 0.1

typedef matrix<double,1,1> sample_type; // 1x1 [temp]
typedef radial_basis_kernel<sample_type> kernel_type;

kcentroid<kernel_type> kcentroidTest(kernel_type(ACCURACY),0.005, 30);

bool load = false;

int i=0;
double convertTimeToInt(char* time){
        char* week = (char*) malloc(100);
        int day = 0;
        int hrs = 0;
        int mins = 0;
        int secs = 0;
        int year = 0;
        sscanf(time, "%s %s %d %d:%d:%d %d",week,week,
                                &day,&hrs, &mins, &secs,&year);
        double seconds = hrs*60*60 + mins*60 + secs;
        return seconds;
}

void insertData(Data* data){
	sample_type m;
	m(0) = data->temperature;
	kcentroidTest.train(m);
}

void loadData(){
	Data* data = (Data*) malloc(sizeof(Data)*sizeOfKnow);
	int numrows = loadLastsDatas(data,sizeOfKnow);
	for(int i=0;i<numrows;i++){
		insertData(&data[i]);
	}
}

double testKCentroid(Data* data){
	if(!load){
		loadData();
		load = true;
	}
	sample_type m;
        m(0) = data->temperature;
	return kcentroidTest(m) > limit? 2 : 0;
}
