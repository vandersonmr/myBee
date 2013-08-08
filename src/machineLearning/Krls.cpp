#include "include/Krls.h"

Krls Krls::krlsControl;

Krls::Krls() : krlsTest(kernel_type(ACCURACY),0.001), ManipulateData(){}

double Krls::convertTimeToSeconds(char* time){
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

void Krls::insertData(Data* data){
	sample_type m;
	m(0) = convertTimeToSeconds(data->time);
	krlsTest.train(m,data->temperature);
}

int Krls::estimate(Data *data){
	sample_type m;
	m(0) = convertTimeToSeconds(data->time);
	cout << "Temperatura esperada: " << krlsTest(m) << endl;
	cout << "Temperatura real: " << data->temperature << endl;
	return 4;
}
