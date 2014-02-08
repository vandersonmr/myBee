#include <chrono>
#include <thread>
#include "include/client_monitor.hpp"

void ClientMonitor::GeneratorsRunner() {
  while (is_running) { 
    if (data_generators.size() == 0)
      continue;

    vector<Data> data;
    for (auto generator : data_generators) { 
      data.push_back(generator.second());
    }

    vector<string> interests = {"server"}; 

    message<Data> msg;
    msg.data      = data;
    msg.interests = interests;

    repa_api.send_message(msg);  

    this_thread::sleep_for(chrono::seconds(freq));
  }
}

ClientMonitor::ClientMonitor(string name, int freq) {
  this->freq = freq;
  this->is_running = true;
  this->name = name;
  thread (&ClientMonitor::GeneratorsRunner,this).detach(); 

  vector<string> interests = {"client"};

  repa_api.init_repa(interests);
} 

void ClientMonitor::AddDataGenerator(string name, function<Data(void)> callback) {
  data_generators[name] = callback;
}

void ClientMonitor::RmDataGenerator(string name) {
  data_generators.erase(name);
}

void ClientMonitor::SetFreq(int freq) {
  this->freq = freq;
}

void ClientMonitor::Close() {
  this->is_running = false;
  repa_api.close_repa();
}
