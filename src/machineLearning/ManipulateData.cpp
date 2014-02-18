#include "include/ManipulateData.h"

void ManipulateData::checkLoad(string nodeName, string type){
    if(!load){
        load = true;
        loadData(nodeName, type);
    }
}

void ManipulateData::loadData(string nodeName, string type){
    std::vector<Data> datas = loadLastsDatasByType(sizeOfKnow,nodeName,type);
    fillData(datas);
}

void ManipulateData::fillData(std::vector<Data> datas){
    for (Data data : datas)
        insertData(data);
}

int ManipulateData::testData(Data data, string nodeName){
    checkLoad(nodeName, data.type);
    return estimate(data);
}
