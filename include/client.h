#ifndef CLIENT_H
#define CLIENT_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <map>
#include <cmath>
#include <thread>
#include <iostream>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include "reproduction.h"
#include "physics.h"
#include "creature.h"
#include "brain.h"
#include "threadsmg.h"
#include "filemanager.h"

typedef websocketpp::client<websocketpp::config::asio_client> client;


class SimpleClient {
public:
    SimpleClient(const std::string &uri, const std::string path);
    ~SimpleClient();
    void send(Packet pck);
    void run(SimTasker* stk);
    int state = 0;
    std::string sbfpath = "";
    std::string srvid = "";
    std::vector<std::string> selectioned;
    std::vector<float> scores;

private:
    client m_client;
    websocketpp::connection_hdl m_hdl;
    std::thread m_thread;
    SimTasker* mstk;
};

int simulate(SimTasker stk, bool mc = false, SimpleClient* cl = nullptr);


#endif // CLIENT_H