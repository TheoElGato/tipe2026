#include "server.hpp"

/*
    I will list all of the packets here :
    
    From the Client : 
    - connect; hostname, null, null : register the client with hostname to the server
    - genfinished; generation, json_results, null : sending generation results to the server
    - simfinished; null, null, null : client confirm to the server he have finished his sim.
    
    From the Server : 
    - connected; id, null, null : confirm the registration with the id of the client
    - startsim; task_id, null, null : ask clients to start task #task_id
    - nextgen; json_selectioned_files, json_scores, null : send to clients the files and scores to load to start a new gen
    - stopsim; null, null, null : ask the clients to stop their current task
    - exit; null, null, null : shutdown the client

*/

LogicServer::LogicServer(std::string sbf_path) {
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
            finished[from] = false;
        	
        	Packet resp("connected",std::to_string(from),"","");
        	send(resp, hdl);
        	nb_client +=1;
        	logm("[+] Client#"+std::to_string(from)+": "+ clients_hn[from] +" connected","Server");
        }
        if (r.cmd == "genfinished") {
            std::vector<float> result = jsonstring_to_vectf(r.arg2);
            logm("Client#"+std::to_string(from)+": "+ clients_hn[from] +" finished gen #"+r.arg1,"Server");
            finished[from] = true;

            if (result.size() == 0) {
                logm("Client#"+std::to_string(from)+": "+clients_hn[from]+" send a result of 0 on a generation","ERROR");
                genresults.emplace_back(std::vector<float>{}, std::vector<Brain>{});
            } else {
                std::vector<Brain> vb;
                for (int i=0; i<result.size(); i++) {
                    Brain db(1,1,"","cpu",1); // Dummy Brain
                    db.bid1 = from;
                    db.bid2 = i;
                    vb.push_back(db);
                }
                genresults.emplace_back(result, vb);
            }

            if (step == 1) {
                timetime = std::time(nullptr);
                step = 2;
            }
            cfinished +=1;
        }
        
        if (r.cmd == "simfinished") {
            logm("Client#"+std::to_string(from)+": "+ clients_hn[from] +" finished task","Server");
            cfinished += 1;
        }
        
    });
    sbfpath = sbf_path+"/";
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

void LogicServer::handle_input() {

    while (true) {
     
        char input[25];
        std::cin.get(input, 25);
        
        std::string sinput = input;

        if (sinput.find("kill") != std::string::npos) {
            logm("Recive kill command");
            Packet exitpck("exit","","","");
            send_all(exitpck);
        }
    }

}


void LogicServer::logic_loop() {
    // We are waiting for clients to connect
    logm("Press ENTER when all clients are connected sucessfully");
    std::getc(stdin);
    
    // Launching input thread in background
    std::thread([this]() { this->handle_input(); }).detach();
    
    logm("All clients are connected. Starting Logic Loop...");
    
    SimDataStruct sds("save","",0,0,0,0,0,0,0,1,true);

	int task_done = 0;
	int started_at = 0;
	int gen_started_at = 0;
	int generation = 0;
    float seperation_time = 0;

    while (task_done != mstk->len) {

    	if (step == 0) { // Asking client to start a sim
    		
    		// Load the right task
    		mstk->loadTask(task_done);
    		
    		// Creating sds
    		sds = SimDataStruct("save",mstk->sim_name,0,mstk->sous_sim,0,mstk->evolution,mstk->brain_acc,mstk->nb_brain,mstk->nb_agent,1);
            sds.save();
    		
    		logm("Asking clients to start sim #"+std::to_string(task_done));
    		Packet startpacket("startsim",std::to_string(task_done),"","");
    		send_all(startpacket);
    		
    		// Get a time stamp
    		started_at = std::time(nullptr);
    		gen_started_at = std::time(nullptr);
            timeout = mstk->sim_time*2;  // 100% more time than the sim time

            for (auto &pair : connections){
                finished[pair.second] = false;
            }
    		
    		genresults.clear(); // We never know...
    		generation = 0;
    		logm("Starting new generation #"+std::to_string(generation));
    		step = 1;
    	}
    	
    	if (step==1) {
    	    // Clients are working on a generation. We are waiting...
    	    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    	}
    	
    	if (step==2) { // One client have finished.
    	    if (cfinished==nb_client) step=3;
    	    else if(std::time(nullptr)>(timetime+timeout)) {
    	        logm("Some clients need to be kicked. Reason : timeout","WARNING");

                // /!\ DANGER /!\
                // this NEED testing to ensure it works as intended and does not kick wrong clients
                // /!\ DANGER /!\

                // Send "exit" to any connected client that is not in finished
                Packet exitpck("exit","","","");
                for (auto &pair : connections) {
                    logm(std::to_string(pair.first) + std::to_string(finished[pair.second]));
                    if (! finished[pair.second]) {
                       send(exitpck, pair.first);
                    }
                }

                step=3;
    	    }
    	}
    	
    	if (step==3) { // Now we analyse the results here
    	    logm("Generation done. Processing...");

            seperation_time = std::time(nullptr) - timetime;
            float processing_time = std::time(nullptr);
    	    
    	    std::vector<float> allFloats;
            std::vector<Brain> allBrains;
            
            // Reserve space to avoid reallocations (optional but faster)
            size_t totalFloats = 0;
            size_t totalBrains = 0;
            
            for (auto &pair : genresults) {
                totalFloats += pair.first.size();
                totalBrains += pair.second.size();
            }
            
            allFloats.reserve(totalFloats);
            allBrains.reserve(totalBrains);
            
            // Merge
            // I'm concerned by the complexity of this...
            // We have to test it but I think we need to rework all of this...
            for (auto &pair : genresults) {
                allFloats.insert(allFloats.end(), pair.first.begin(), pair.first.end());
                allBrains.insert(allBrains.end(), pair.second.begin(), pair.second.end());
            }
            
            // Sort by score descending
            reverse_sorting_brain(&allBrains, &allFloats, 0, allBrains.size() - 1);

            // Saving data
            int best_score_index = std::distance(allFloats.begin(), std::max_element(allFloats.begin(), allFloats.end())); 
            float generationTemp = generation;
            float agent0scoreTemp= allFloats[0];
            float agent1scoreTemp= allFloats[1];
            float agent2scoreTemp= allFloats[2];
            float agent3scoreTemp= allFloats[3];
            float agent4scoreTemp= allFloats[4];
            float agent5scoreTemp= allFloats[5];
            float agent6scoreTemp= allFloats[6];
            float agent7scoreTemp= allFloats[7];
            float agent8scoreTemp= allFloats[8];
            float agent9scoreTemp= allFloats[9];
            float meanTemp = average(allFloats);
            float bestAgentScoreTemp = allFloats[0];
            float timeForOneGenTemp = std::time(nullptr)-gen_started_at;
            int midid = allFloats.size()/2;
            float median = allFloats[midid];
            
            ////
            
            // Selected whitch files need to be send to the clients
            // The bests 

            packageSelectionned.clear();
            packageScores.clear();
            
            for(int j = 0; j < 1+nb_client/2; j+=1){
                std::vector<std::string> selectioned;
                std::vector<float> scores;
                for(int i=0; i<(mstk->nb_brain); i+=1) {
                    std::string idstr = std::to_string(allBrains[i+j*(mstk->nb_brain)].bid1)+"s"+std::to_string(allBrains[i+j*(mstk->nb_brain)].bid2)+".pt";
                    selectioned.push_back(idstr);
                    scores.push_back(allFloats[i]);
                }

                // Packaging it in json to send it
                packageSelectionned.push_back(vects_to_jsonstring(selectioned));  
                packageScores.push_back(vectf_to_jsonstring(scores));           
            }
            
            // Making a copy of the best brains
            for (int i=0;i<mstk->nb_brain;i+=1) {
                std::string idstr = std::to_string(allBrains[i].bid1)+"s"+std::to_string(allBrains[i].bid2)+".pt";

                std::ifstream  src(sbfpath+idstr,std::ios::binary);
                std::string istring = std::to_string(i);
                std::ofstream  dst(sds.getFullPath()+istring+".pt",std::ios::binary);
                dst << src.rdbuf();
            }

            processing_time = std::time(nullptr) - processing_time;

            sds.addStatRow(generationTemp, agent0scoreTemp, agent1scoreTemp, agent2scoreTemp, 
               agent3scoreTemp, agent4scoreTemp, agent5scoreTemp, agent6scoreTemp,
               agent7scoreTemp, agent8scoreTemp, agent9scoreTemp, meanTemp, median,
               bestAgentScoreTemp,timeForOneGenTemp,seperation_time,processing_time,std::time(nullptr), nb_client);
           
            sds.data["generation"] = generation;
            sds.data["simu_time"] = mstk->sim_time;
            sds.data["evolution"] = mstk->evolution;
            sds.data["total_trained_time"] = (std::time(nullptr) - started_at);
            sds.save();

            
            logm("Autosaved.", "SAVE");

            // Clearing memory
            // (I think this is not necessery but why not :/
            allFloats.clear();
            allBrains.clear();
    	    
    	    step=4;
    	}
    	
    	if (step==4) {
    	
    	    if(!mstk->is_infinite &&  (std::time(nullptr) - started_at)>mstk->time_allowed) {
    	        logm("Time limit reached. Finishing this task now.");
    	        
    	        // Cleaning up
        	    cfinished = 0;
                genresults.clear();
    	        
    	        Packet stpsim("stopsim","","","");
    	        send_all(stpsim);
    	        step=5;
    	        
    	    } else {
    	
                generation += 1;
                gen_started_at = std::time(nullptr);
        	    logm("Starting new generation #"+std::to_string(generation));
        	    
        	    // Cleaning up
        	    cfinished = 0;
                genresults.clear();

                // Sending nextgen packet
                
                
                int half_clients = 1;

                if(nb_client == 1){
                    half_clients = 1;
                } else {
                    half_clients = nb_client/2;
                }
                
                logm("Sending nextgen to Clients");
                int i = 0;
                for (auto &pair : connections) {
                    Packet pck("nextgen",packageSelectionned[i%half_clients],packageScores[i%half_clients],"");
		            send(pck, pair.first);
                    i += 1;
                }

    
        	    step = 1;
        	}
        	
    	}
    	
    	if (step==5) {
    	    // Waiting for all clients to stop their sim
    	    if (cfinished != nb_client) continue;
    	    
    	    // We are done
    	    cfinished = 0;
    	    task_done += 1;
    	    step = 0;
    	}
    }
    
    logm("There was" + std::to_string(nb_client) + " clients connected", "INFO");

    this->running = false;

    logm("Asking all clients to shutdown...");
    Packet exitpck("exit","","","");
    send_all(exitpck);
    
    while(nb_client > 0) continue;
    logm("Server Logic Loop finished. WS Server will stop now.");
    m_server.stop();
}
