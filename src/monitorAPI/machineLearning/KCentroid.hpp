#ifndef KCENTROID_H
#define KCENTROID_H
#include "ManipulateData.hpp"

#define limit 1

template <typename T>
class KCentroid: public ManipulateData<T> {
  protected:
    typedef typename ManipulateData<T>::sample_type sample_type;
    typedef typename ManipulateData<T>::kernel_type kernel_type;
    kcentroid<kernel_type> kcentroidTest;
    void insertData(Data<T>);
    int estimate(Data<T>);

  public:
    KCentroid();
    ~KCentroid(){}
};

template <typename T>
KCentroid<T>::KCentroid() : ManipulateData<T>(), 
  kcentroidTest(kernel_type(ACCURACY),0.005, 30){}

template <typename T>
void KCentroid<T>::insertData(Data<T> data){
    sample_type m;
    m(0) = data.value;
    kcentroidTest.train(m);
}

template <typename T>
int KCentroid<T>::estimate(Data<T> data){
    sample_type m;
    m(0) = data.value;
    return kcentroidTest(m) > limit? 2 : 0;
}

#endif
