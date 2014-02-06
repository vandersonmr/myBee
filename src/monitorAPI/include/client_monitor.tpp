#include <chrono>
#include <thread>

template<class T>
void ClientMonitor<T>::GeneratorsRunner() {
  while (is_running) { 
    if (data_generators.size() == 0)
      continue;
  
    vector<T> data;
    for (auto generator : data_generators) { 
      data.push_back(generator.second());
    }

    vector<string> interests = {"server"}; 

    message<vector<T>> msg;
    msg.data      = data; 
    msg.interests = interests;
  
    repa_api.send_message(msg);  

    this_thread::sleep_for(chrono::seconds(freq));
  }
}

template<class T>
ClientMonitor<T>::ClientMonitor(string name, int freq) {
  this->freq = freq;
  this->is_running = true;
  thread th([this] { GeneratorsRunner(); }); 

  vector<string> interests = {"client"};

  repa_api.init_repa(interests);
} 

template<class T>
void ClientMonitor<T>::AddDataGenerator(string name, function<T(void)> callback) {
  data_generators[name] = callback;
}

template<class T>
void ClientMonitor<T>::RmDataGenerator(string name) {
  data_generators.erase(name);
}

template<class T>
void ClientMonitor<T>::SetFreq(int freq) {
  this->freq = freq;
}

template<class T>
void ClientMonitor<T>::Close() {
  this->is_running = false;
  repa_api.close_repa();
}
