#include <dataDAO.h>
#include <iostream>
#include <vector>

#include <dlib/svm.h>

using namespace std;
using namespace dlib;

typedef matrix<double,1,1> sample_type; // 1x1 [temp]
typedef radial_basis_kernel<sample_type> kernel_type;

std::vector<sample_type> samples;
std::vector<double> labels;

kcentroid<kernel_type> kcentroidTest(kernel_type(ACCURACY),0.01, 15);


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
