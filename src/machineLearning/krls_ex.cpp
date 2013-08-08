#include <iostream>
#include <vector>
#include <dataDAO.h>
#include <dlib/svm.h>

using namespace std;
using namespace dlib;

#define accuracy 0.1

typedef matrix<double,1,1> sample_type; // 1x1 [temp]
typedef radial_basis_kernel<sample_type> kernel_type;

krls<kernel_type> krlsTest(kernel_type(accuracy),0.001);


int i=0;

void insertData(Data* data){
	sample_type m;
	m(0) = convertTimeToInt(data->time);
	krlsTest.train(m,data->temperature);
}

double instimate(Data* data){
	sample_type m;
        m(0) = convertTimeToInt(data->time);
	return krlsTest(m);
}

int main(){
	Data* data = (Data*) malloc(sizeof(Data));
	data->temperature = 35;
	insertData(data);
	return 0;
}
