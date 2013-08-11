#include "include/ManipulateData.h"

void ManipulateData::checkLoad(){
	if(!load){
		load = true;
		loadData();
	}
}

double ManipulateData::convertTimeToSeconds(char* time){
	char* week = (char*) malloc(100);
	int day = 0;
	int hrs = 0;
	int mins = 0;
	int secs = 0;
	int year = 0;
	sscanf(time, "%s %s %d %d:%d:%d %d",week,week,
			&day,&hrs, &mins, &secs,&year);
	int seconds = hrs*60*60 + mins*60 + secs;
	return seconds % 314;// (((2*pi)/0.1)*5)
}

void ManipulateData::loadData(){
	Data* data;
	int numrows = loadLastsDatas(&data,sizeOfKnow);
	fillData(data,numrows);
}

void ManipulateData::fillData(Data* data, int numrows){
	for(int i=0;i<numrows;i++){
		insertData(&data[i]);
	}
}

int ManipulateData::testData(Data* data){
	checkLoad();
	return estimate(data);
}
