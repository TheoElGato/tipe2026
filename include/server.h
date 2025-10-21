#ifndef SERVER_H
#define SERVER_H

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "filemanager.h"
#include <map>
#include <string>
#include <iostream>
#include <unordered_map>
#include <atomic>

typedef websocketpp::server<websocketpp::config::asio> server;

class LogicServer {
public:
    LogicServer();
    void run(uint16_t port, SimTasker* test);
    
     

private:
    server m_server;
    std::atomic<uint64_t> next_id = 0;
	std::map<websocketpp::connection_hdl, uint64_t, std::owner_less<websocketpp::connection_hdl>> connections;
	std::map<uint64_t, std::string> clients_hn;
	SimTasker* mstk;
	void logic_loop();
	void send_all(Packet pck);
	void send(Packet pck, websocketpp::connection_hdl hdl);

};

	
#endif // SERVER_H