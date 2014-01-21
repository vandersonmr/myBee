#include <msgpack.hpp>
#include <cstdlib>
#include <iostream>
#include <signal.h>

template<class T>
void* RepaAPI<T>::handle_message(void*) {
  terminated = false;
  while (!terminated) {
    message<T> msg = get_message();
    callback(msg);
  }
  return 0; 
}

template<class T>
bool RepaAPI<T>::init_repa(vector<string> interests) {
  sock = repa_open(); 
  for (string interest : interests) 
    repa_register_interest(sock,interest.c_str());
  return true;
}

template<class T>
message<T> RepaAPI<T>::get_message() {
  prefix_addr_t prefix_addr;
  const char* data = (char*)malloc(1500*sizeof(char));
  char* interest = (char*)malloc(255*sizeof(char));

  int read_len = repa_timed_recv(sock,interest, data, prefix_addr, (long int)1E9);

  message<T> result;
  if (read_len > 0) {
    msgpack::unpacked msg;
    msgpack::unpack(&msg, data, read_len);
    msgpack::object obj = msg.get();

    obj.convert(&result);

  }

  free(interest);
  return result;
}

template<class T>
void RepaAPI<T>::get_message(function<void(message<T>)> callback) {
  this->callback = callback;
  pthread_create(&thread, NULL, &RepaAPI::runHelper, this);
}


template<class T>
bool RepaAPI<T>::send_message(message<T> msg) {
  for (string interest : msg.interests) {
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, msg);

    repa_send(sock, interest.c_str(), sbuf.data(), sbuf.size(), 0);
  }
  return true;
}

template<class T>
bool RepaAPI<T>::close_repa() {
  terminated = true;
  if(pthread_kill(thread, 0) == 0)
  {
    /* still running */
    pthread_join(thread, NULL);
  }
  repa_close(sock);
  exit(EXIT_SUCCESS);
}

