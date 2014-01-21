#include "../../repa/hdr/repa.h"
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
    
    void* handle_message(void*);

    static void *runHelper(void *classRef){
      return ((RepaAPI *)classRef)->handle_message(classRef);
    }

  public:
    bool init_repa (vector<string>);
    message<T> get_message();
    void get_message(function<void(message<T>)> callback);
    bool send_message(message<T>);
    bool close_repa();
};
#include "repaAPI.tpp"
