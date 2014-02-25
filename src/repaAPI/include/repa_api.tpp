#include <msgpack.hpp>
#include <cstdlib>
#include <iostream>
#include <signal.h>

template<class T>
void* RepaAPI<T>::HandleMessage(void*) {
    terminated = false;
    while (!terminated) {
        message<T> msg = GetMessage();
        callback(msg);
    }
    return 0; 
}

template<class T>
bool RepaAPI<T>::InitRepa(vector<string> interests) {
    sock = repa_open();
    if (sock.error < 0){
      cout << "Protocolo repa não inicializado." << endl;
      return false;
    }
    for (string interest : interests)
        repa_register_interest(sock,interest.c_str());
    return true;
}

template<class T>
message<T> RepaAPI<T>::GetMessage() {
    prefix_addr_t prefix_addr;
    char* data = new char[1500];
    char* interest = new char[255];

    int read_len = repa_recv(sock,interest, data, prefix_addr);

    message<T> result;
    if (read_len > 0) {
        msgpack::unpacked msg;
        msgpack::unpack(&msg, data, read_len);
        msgpack::object obj = msg.get();

        obj.convert(&result);

        char* prefix = new char[255];
        repa_print_prefix(prefix_addr,prefix);
        result.prefix_address = string(prefix);
        delete[] prefix;
    }
    
    delete[] data;
    delete[] interest;
    return result;
}

template<class T>
void RepaAPI<T>::GetMessage(function<void(message<T>)> callback) {
    this->callback = callback;
    pthread_create(&thread, NULL, &RepaAPI::RunHelper, this);
}


template<class T>
bool RepaAPI<T>::SendMessage(message<T> msg) {
    for (string interest : msg.interests) {
        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, msg);

        repa_send(sock, interest.c_str(), sbuf.data(), sbuf.size(), 0);
    }
    return true;
}

template<class T>
vector<string> RepaAPI<T>::GetNodesOnline() {
    struct dllist *list = NULL;
    struct dll_node *lnode = NULL;
    char* prefix = new char[255];

    dll_create(list);

    repa_get_nodes_in_network(sock, list);

    vector<string> nodes;

    repa_print_prefix(repa_get_node_address(),prefix);
    nodes.push_back(prefix);

    for(lnode = list->head; lnode != NULL; lnode = lnode->next) {
        repa_print_prefix((intptr_t) lnode->data, prefix);
        nodes.push_back(prefix);
    }

    delete[] prefix;
    dll_destroy(list);
    return nodes;
}

template<class T>
bool RepaAPI<T>::CloseRepa() {
    terminated = true;
    repa_close(sock);
    if(pthread_kill(thread, 0) == 0)
    {
        /* still running */
        pthread_join(thread, NULL);
    }
    exit(EXIT_SUCCESS);
}
