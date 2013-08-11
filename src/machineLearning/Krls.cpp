#include "include/Krls.h"

Krls Krls::krlsControl;

Krls::Krls() : krlsTest(kernel_type(ACCURACY),0.0001), ManipulateData(){}

void Krls::insertData(Data* data){
	sample_type m;
	m(0) = convertTimeToSeconds(data->time);
	krlsTest.train(m,data->temperature);
}

int Krls::estimate(Data *data){
	sample_type m;
	m(0) = convertTimeToSeconds(data->time);
	cout << "Krls -> Temperatura esperada: " << krlsTest(m) << endl;
	cout << "Krls -> Temperatura real: " << data->temperature << endl;
	int status = abs(krlsTest(m) - (double) data->temperature) > LIMIT? 4 : 0;
	insertData(data);
	return status;
}
