#include <iostream>
#include <vector>

#include <dlib/svm.h>
#include <dlib/statistics.h>
#include <dataDAO.h>

using namespace std;
using namespace dlib;

#define ACCURACY 0.1

typedef matrix<double,2,1> sample_type; // 2x1 [time,temp]
typedef radial_basis_kernel<sample_type> kernel_type;

kcentroid<kernel_type> kcentroidTest(kernel_type(ACCURACY),0.01, 15);


int i=0;
int convertTimeToInt(char* time){
	return i++; 
}

void insertData(Data* data){
	sample_type m;
	m(0) = convertTimeToInt(data->time);
	m(1) = data->temperature;
	kcentroidTest.train(m);
}

double test(Data* data){
	sample_type m;
        m(0) = convertTimeToInt(data->time);
        m(1) = data->temperature;
	return kcentroidTest(m);
}

int main(){
	Data* data = (Data*) malloc(sizeof(Data));
	data->temperature = 35;
	insertData(data);
	return 0;
}
