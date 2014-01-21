#include "include/ManipulateData.h"

void ManipulateData::checkLoad(string nodeName){
	if(!load){
		load = true;
		loadData(nodeName);
	}
}

void ManipulateData::loadData(string nodeName){
  std::vector<Data> datas = loadLastsDatas(sizeOfKnow,nodeName);
	fillData(datas);
}

void ManipulateData::fillData(std::vector<Data> datas){
	for (Data data : datas)
		insertData(data);
}

int ManipulateData::testData(Data data, string nodeName){
	checkLoad(nodeName);
	return estimate(data);
}
