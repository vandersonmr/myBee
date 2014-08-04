#include <iostream>
#include "monitorAPI/include/server_monitor.hpp"

ServerMonitor server;

Data filter(Data d) {
    cout << " Prefix: " <<
        d.nickname << " send a msg " << d.type << endl;
    return d;
}

void handler(int sig){
    cout << "Signal " << sig << endl;
    server.Close();
}

int main(void) {
    server.EnablePersistence("../../config/db.conf");
    server.EnableMachineLearning(1);
    server.SetFilter(&filter);
    
    signal(SIGINT, &handler);

    while(true) sleep(1);

    return 0;
}
