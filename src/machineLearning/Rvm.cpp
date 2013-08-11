#include "include/Rvm.h"


Rvm::Rvm() : ManipulateData() {
	rvmTest.set_epsilon(0.001);
}

void Rvm::fillData(Data* data, int numrows){
	if (numrows < AMOUNT){
		load = false;
		return;
	}

	for (int i = numrows - 1; i > numrows - AMOUNT; i--){
		insertData(&data[i]);
	}
	trainData();
}

void Rvm::insertData(Data* data){
	sample_type m;
	m(0) = convertTimeToSeconds(data->time);
	samples.push_back(m);
	labels.push_back(data->temperature);
}

void Rvm::trainData(){
	gamma = 1.0/compute_mean_squared_distance(samples);
	rvmTest.set_kernel(kernel_type(gamma));
	rvmFunction = rvmTest.train(samples,labels);
}

void Rvm::pushNewData(Data* data, sample_type m){
	samples.pop_back();
	labels.pop_back();
	samples.insert(samples.begin(),m);
	labels.insert(labels.begin(),data->temperature);
	trainData();
}

int Rvm::estimate(Data* data){
	if (!load) return 0;
	sample_type m;
	m(0) = convertTimeToSeconds(data->time);
	cout << "Rvm -> Estimate: " << rvmFunction(m) << endl;
	cout << "Rvm -> Real: " << data->temperature << endl;
	int status = abs(rvmFunction(m) - (double) data->temperature) > LIMIT? 8 : 0;
	pushNewData(data,m);
	return status;
}
