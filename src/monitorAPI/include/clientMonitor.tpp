#include <chrono>
#include <thread>

template<class T>
ClientMonitor<T>::generators_runner() {
  while (isRunning) { 
    if (dataGenerators.size() == 0)
      continue
  
    vector<T> data;

    for (auto generator : dataGenerators) 
      data.push_back(generator());

    vector<string> interests; 
    interests.push_back("server");

    message<vector<T>> msg;
    msg.data      = data; 
    msg.interests = interests;
  
    repaAPI.send_message(msg);  

    this_thread::sleep_for(chrono::seconds(freq));
  }
}

template<class T>
ClientMonitor<T>::ClientMonitor(string name, int freq) {
  this->freq = freq;
  this->isRunning = true;
  thread th(generators_runner); 

  vector<string> interests;
  interests.push_back(string("client"));

  repaAPI.init_repa(interests);
} 

template<class T>
void ClientMonitor<T>::add_data_generator(string name, function callback) {
  dataGenerators[name] = callback;
}

template<class T>
void ClientMonitor<T>::rm_data_generator(string name) {
  dataGenerators.erase(name);
}

template<class T>
void ClientMonitor<T>::set_freq(int freq) {
  this->freq = freq;
}

template<class T>
void ClientMonitor<T>::close() {
  this->isRunning = false;
  repaAPI.close_repa();
}
