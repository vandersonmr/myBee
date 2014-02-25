#ifndef REPAAPI
#define REPAAPI

#include "../../repa/hdr/repa.h"
#include "../../repa/hdr/linkedlist.h"
#include "message.hpp"
#include <string>
#include <vector>
#include <functional>

using namespace std;

template<class T>class RepaAPI {
  private:
    bool terminated       = true;
    pthread_t thread;
    repa_sock_t sock;
    function<void(message<T>)> callback;
    
    void* HandleMessage(void*);

    static void *RunHelper(void *class_ref){
      return ((RepaAPI *)class_ref)->HandleMessage(class_ref);
    }
  public:
    bool InitRepa (vector<string>);
    message<T> GetMessage();
    void GetMessage(function<void(message<T>)> callback);
    bool SendMessage(message<T>);
    vector<string> GetNodesOnline(); 
    bool CloseRepa();
};
#include "repa_api.tpp"

#endif
