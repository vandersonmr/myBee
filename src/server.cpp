#include <iostream>
#include "monitorAPI/include/server_monitor.hpp"

Data filter(Data d) {
    cout << " Prefix: " <<
        d.nickname << " send a msg " << d.type << endl;
    return d;
} 

int main(void) {
    ServerMonitor server;

    server.EnablePersistence("config/db.conf");
    server.EnableMachineLearning(1);
    server.SetFilter(&filter);

    string word;
    string end = "quit";
    
    while(word.compare(end) != 0) std::cin >> word;

    server.Close();
    return 0;
}
