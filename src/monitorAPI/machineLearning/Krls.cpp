#include "include/Krls.h"

Krls::Krls() : ManipulateData(), krlsTest(kernel_type(ACCURACY), 0.0001) {}

int getTime(Data data) {
    auto time = gmtime(&data.time); 
    return time->tm_hour*60 + time->tm_min; 
}

void Krls::insertData(Data data){
    sample_type m;
    m(0) = getTime(data);
    krlsTest.train(m, data.value);
}

int Krls::estimate(Data data){
    sample_type m;
    m(0) = getTime(data);
    cout << "Krls -> " << data.type << " esperada: " << krlsTest(m) << endl;
    cout << "Krls -> " << data.type << " real: " << data.value << endl;
    int status = abs(krlsTest(m) - (double) data.value) > LIMIT? 4 : 0;
    insertData(data);
    return status;
}
