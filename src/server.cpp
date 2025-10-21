#include "server.h"

LogicServer::LogicServer() {
    m_server.init_asio();
    m_server.clear_access_channels(websocketpp::log::alevel::all);
	m_server.clear_error_channels(websocketpp::log::elevel::all);
    m_server.set_open_handler([this](websocketpp::connection_hdl hdl) {
    	uint64_t id = next_id++;
        connections[hdl] =  id;
    });

    m_server.set_close_handler([this](websocketpp::connection_hdl hdl) {
        auto it = connections.find(hdl);
        if (it != connections.end()) {
        	uint64_t from = it->second;
        	logm("[-] Client#"+std::to_string(from)+": "+ clients_hn[from] +" disconnected","Server");
            connections.erase(it);
            clients_hn.erase(from);
            nb_client -=1;
        }
    });

    m_server.set_message_handler([this](websocketpp::connection_hdl hdl, server::message_ptr msg) {
        auto it = connections.find(hdl);
        if (it == connections.end()) return;
        
        uint64_t from = it->second;
        std::string payload = msg->get_payload();
        Packet r(payload);
        
        if (r.cmd == "connect") {
        	clients_hn[from] = r.arg1;
        	
        	Packet resp("connected",std::to_string(from),"","");
        	send(resp, hdl);
        	nb_client +=1;
        	logm("[+] Client#"+std::to_string(from)+": "+ clients_hn[from] +" connected","Server");
        }
        if (r.cmd == "genfinished") {
            if (step == 1) {
                step = 2;
                timetime = std::time(nullptr);
            }
            logm("Client#"+std::to_string(from)+": "+ clients_hn[from] +" finished gen #"+r.arg1,"Server");
            cfinished +=1;
        }

        
    });
}

void LogicServer::send(Packet pck, websocketpp::connection_hdl hdl) {
    websocketpp::lib::error_code ec;
    m_server.send(hdl, pck.get_string(), websocketpp::frame::opcode::text, ec);
    if (ec) {
        logm("Send failed: " + ec.message(),"ERROR");
    }
}

void LogicServer::send_all(Packet pck) {
	// Broadcast to all
    for (auto &pair : connections) {
		send(pck, pair.first);
    }
}

void LogicServer::run(uint16_t port,SimTasker* test) {
    m_server.listen(port);
    m_server.start_accept();
    logm("Server listening on port "+ std::to_string(port) +"...","Server");
    
    mstk = test;
    // Launch the logic loop in another thread
    std::thread([this]() { this->logic_loop(); }).detach();
    
    m_server.run();
}

void LogicServer::logic_loop() {
    // We are waiting for clients to connect
    logm("Press ENTER when all clients are connected sucessfully");
    std::getc(stdin);
    
    logm("All clients are connected Starting Logic Loop...");


	int task_done = 0;
	int started_at = 0;

    while (task_done != mstk->len) {

    	if (step == 0) { // Asking client to start a sim
    		logm("Asking clients to start sim #"+std::to_string(task_done));
    		Packet startpacket("startsim",std::to_string(task_done),"","");
    		send_all(startpacket);
    		started_at = std::time(nullptr);
    		step += 1;
    	}
    	
    	if (step==1) {
    	    // Client are working on a generation. We are waiting...
    	}
    	
    	if (step==2) { // One client have finished.
    	    if (cfinished==nb_client) step +=1;
    	    else if(std::time(nullptr)>(timetime+timeout)) {
    	        logm("Some clients need to be kicked. Reason : timeout","WARNING");
    	        logm("KIKING NOT IMPLEMENTED","WARNING");
    	        step +=1;
    	    }
    	}
    	
    	if (step==3) { // Now we analyse the results here
    	    logm("Generation done. Analysing...");
    	    // TODO
    	    step+=1;
    	}
    	
    	if (step==4) {
    	    cfinished = 0;
    	    Packet ngen("nextgen","","","");
    	    send_all(ngen);
    	    step = 1;
    	}
    }
    
    logm("Server Logic Loop finished. WS Server will continue to run. Please exit using Ctrl+C");
}