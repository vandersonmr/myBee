#include "include/Krls.h"


Krls::Krls() : krlsTest(kernel_type(ACCURACY),0.0001), ManipulateData(){}

void Krls::insertData(Data data){
    sample_type m;
    m(0) = cos(data.time);
    krlsTest.train(m,data.value);
}

int Krls::estimate(Data data){
    sample_type m;
    m(0) = cos(data.time);
    cout << "Krls -> " << data.type << " esperada: " << krlsTest(m) << endl;
    cout << "Krls -> " << data.type << " real: " << data.value << endl;
    int status = abs(krlsTest(m) - (double) data.value) > LIMIT? 4 : 0;
    insertData(data);
    return status;
}
