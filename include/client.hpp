#ifndef CLIENT_H
#define CLIENT_H

#include <SFML/System.hpp>
#include <map>
#include <cmath>
#include <thread>
#include <iostream>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include "reproduction.hpp"
#include "physics.hpp"
#include "creature.hpp"
#include "brain.hpp"
#include "threadsmg.hpp"
#include "filemanager.hpp"

typedef websocketpp::client<websocketpp::config::asio_client> client;


class SimpleClient {
public:
    SimpleClient(const std::string &uri, const std::string path);
    ~SimpleClient();
    void send(Packet pck);
    void run(SimTasker* stk,bool hl);
    bool headless = false;
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

int simulate(SimTasker stk, bool mc = false, bool headless = false, SimpleClient* cl = nullptr);


#endif // CLIENT_H