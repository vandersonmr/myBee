#include "include/Rvm.h"


Rvm::Rvm() : ManipulateData() {
    rvmTest.set_epsilon(0.001);
}

void Rvm::fillData(std::vector<Data> datas, int numrows){
    if (numrows < AMOUNT){
        load = false;
        return;
    }

    for (Data data : datas)
        insertData(data);

    trainData();
}

void Rvm::insertData(Data data){
    sample_type m;
    m(0) = data.time;
    samples.push_back(m);
    labels.push_back(data.value);
}

void Rvm::trainData(){
    gamma = 1.0/compute_mean_squared_distance(samples);
    rvmTest.set_kernel(kernel_type(gamma));
    rvmFunction = rvmTest.train(samples,labels);
}

void Rvm::pushNewData(Data data, sample_type m){
    samples.pop_back();
    labels.pop_back();
    samples.insert(samples.begin(),m);
    labels.insert(labels.begin(),data.value);
    trainData();
}

int Rvm::estimate(Data data){
    if (!load) return 0;
    sample_type m;
    m(0) = data.time;
    cout << "Rvm -> Estimate: " << rvmFunction(m) << endl;
    cout << "Rvm -> Real: " << data.value << endl;
    int status = abs(rvmFunction(m) - (double) data.value) > LIMIT? 8 : 0;
    pushNewData(data,m);
    return status;
}
