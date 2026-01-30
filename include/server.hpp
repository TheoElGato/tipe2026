/*
	server.hpp

	Description:
		Header file for server.cpp

	Author:
		R. Benichou
		A. Spadone
*/

#ifndef SERVER_H
#define SERVER_H

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include "filemanager.hpp"
#include <map>
#include <string>
#include <iostream>
#include <unordered_map>
#include <atomic>
#include "brain.hpp"
#include "reproduction.hpp"

typedef websocketpp::server<websocketpp::config::asio> server;

class LogicServer {
public:
	LogicServer(std::string sbf_path);
	void run(uint16_t port, SimTasker* mstk);
	int timeout = 60;

private:
	server m_server;
	std::string sbfpath = "";
	std::atomic<uint64_t> next_id = 0;
	std::map<websocketpp::connection_hdl, uint64_t, std::owner_less<websocketpp::connection_hdl>> connections;
	std::map<uint64_t, std::string> clients_hn;
	std::map<uint64_t, int> finished; // 0 standby; 1 finished; -1 kicked out; 2 1 warning
	SimTasker* mstk;
	int timetime = 0;
	int cfinished = 0;
	int nb_client = 0;
	int active_client = 0;
	int step = 0;
	bool running = true;
	std::vector<std::string> packageSelectionned;
	std::vector<std::string> packageScores;
	std::vector<std::pair<std::vector<float>,std::vector<Brain>>> genresults;
	void logic_loop();
	void send_all(Packet pck);
	void send(Packet pck, websocketpp::connection_hdl hdl);
	void handle_input();

};

#endif // SERVER_H
