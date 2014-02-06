#include "include/Krls.h"


Krls::Krls() : krlsTest(kernel_type(ACCURACY),0.0001), ManipulateData(){}

void Krls::insertData(Data data){
    sample_type m;
    m(0) = cos(data.time);
    krlsTest.train(m,data.type_value["temperature"]);
}

int Krls::estimate(Data data){
    sample_type m;
    m(0) = cos(data.time);
    cout << "Krls -> Temperatura esperada: " << krlsTest(m) << endl;
    cout << "Krls -> Temperatura real: " << data.type_value["temperature"] << endl;
    int status = abs(krlsTest(m) - (double) data.type_value["temperature"]) > LIMIT? 4 : 0;
    insertData(data);
    return status;
}
