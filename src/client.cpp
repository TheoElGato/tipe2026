#include "client.hpp"
#include "displayservice.hpp"

void error(const std::string& message) {
    // Standardized error function
    logm(message, "ERROR");
    assert(false);
}

std::string remove_zero_ts(float value) {
    std::ostringstream oss;
    oss << std::setprecision(8) << std::noshowpoint << value;
    return oss.str();
}

void init_agents_and_brain(int countAgents, int countBrains, int x, int y, std::vector<Creature>* agents, std::vector<Brain>* brains, std::string device, int nb_hidden_layer, std::string brainFile="") {
    // initialise agent and brain related objects

    for(int i=0; i<countAgents; i+=1) {
        agents->emplace_back(x, y, i);
    }

    if (brainFile == ""){
        // Initialize agents with default brain
        for (int i=0; i<countBrains; i+=1) {
            brains->emplace_back(9,6,"",device,nb_hidden_layer);
        }
    } else {
        // Initialize agents with brain from file
        for (int i=0; i<countBrains; i+=1) {
            std::string istring = std::to_string(i);
            logm("Loading brain"+istring+" from file.");
            brains->emplace_back(9,6,brainFile+istring+".pt",device,nb_hidden_layer);
        }
    }
}

int simulate(SimTasker stk, bool mc, bool headless,SimpleClient* cl) {
    
    logm("Welcome to the URSAF Sim");
    
    /// SETTINGS FROM THE SIMTASKER///
    std::string DEVICE = stk.device;
    int THREADS = stk.threads;
    bool LOAD_FROM_FILE = stk.load_from_file;
    std::string LOAD_NAME = stk.load_name;
    std::string SIM_NAME = stk.sim_name;
    int SIM_TIME = stk.sim_time;
    float EVOLUTION = stk.evolution;
    float BRAIN_ACC = stk.brain_acc;
    int NB_BRAIN = stk.nb_brain;
    int NB_AGENT = stk.nb_agent;
    int NB_HIDDEN_LAYER = stk.nb_hidden_layer;
    int SOUS_SIM = stk.sous_sim;
    int BEST_KEEP = stk.best_keep;
    int SELECTION_POL = stk.selection_pol;
    bool AUTOSAVE = stk.autosave;
    int AUTOSAVE_FREQ = stk.autosave_freq;
    int MAXDIST = stk.maxdist;
    int MINDIST = stk.mindist;
    int NB_GOAL = stk.nb_goal;
    sf::Vector2f start = sf::Vector2f(stk.startx, stk.starty);
    bool is_infinite = stk.is_infinite;
    int time_allowed = stk.time_allowed;
    bool use_evolution_curve = stk.use_evolution_curve;
    float curve_a = stk.curve_a;
    float curve_b = stk.curve_b;
    float curve_c = stk.curve_c;
    float curve_d = stk.curve_d;
    ///////////////////

    /// Temp
    float ss_dt = 1/60.f;
    float dt = 0.016f;
    
    ////// Variable related to the state of simulation ///////
    
    // Var simulation gestion
    int nb_agent = NB_AGENT;
    int nb_brain = NB_BRAIN;
    int simu_time = SIM_TIME;
    float evolution = EVOLUTION;
    int generation = 0;
    bool clean_exit = false;

    // Accumulators
    float acc = 0; // Used for the total trained time
    float acu = 0; // Used for the time for a generation 
    int cyl = 0;

    // Some necessery data for brains and agents
    std::vector<Brain> brain_agent;
    std::vector<Creature> agents;
    
    // Init the SimDataStruct
    std::string sim_name = SIM_NAME;
    if (LOAD_FROM_FILE) sim_name=LOAD_NAME;
    SimDataStruct sds("save",sim_name,0, SOUS_SIM,0,evolution, BRAIN_ACC, nb_brain,nb_agent,1,mc);
    
    // If needed, load the sim
    if (LOAD_FROM_FILE) {
        if(mc) logm("Loading in client mode not supported.","WARNING");
        // Load simulation state from file
        logm("Loading the sim","WARNING");
        sds.loadFromFile(LOAD_NAME);
        
        nb_agent = sds.data["agents-number"];
        nb_brain = sds.data["brains-number"];
        generation = sds.data["generation"];
        generation += 1;
        simu_time = sds.data["simu_time"];
        evolution = sds.data["evolution"];
        acc = sds.data["total_trained_time"];
        int temp = sds.data["train_sessions"];
        sds.data["train_sessions"] = temp + 1;
        
        init_agents_and_brain(nb_agent, nb_brain, start.x, start.y, &agents, &brain_agent, DEVICE,NB_HIDDEN_LAYER,"save/"+LOAD_NAME+"/");
    
    } else {
        // Initialize simulation state
        init_agents_and_brain(nb_agent, nb_brain, start.x, start.y, &agents, &brain_agent, DEVICE,NB_HIDDEN_LAYER);
    }

    if (!mc) sds.save();

    // Data for agents and brains
    std::vector<float> score_agent(nb_brain, 0);
    std::vector<float> agents_objectif(nb_agent, 0);
    
    // Data for sous sim
    std::vector<std::thread> sous_sim_threads(SOUS_SIM);
    std::vector<std::vector<float>> sous_sim_scores(SOUS_SIM);

    // State of each sous-sim: 
    // 0 = not started
    // 1 = running, the thread is doing its job
    //     (the main thread will check if it's finished)
    //     (the thread will change its state to 2 when finished)
    // 2 = finished, the thread marked itself as finished
    // 3 = collected, the main thread collected the results and processed them
    std::vector<int> sous_sim_state(SOUS_SIM, 0);
    
    // -1 means available
    // else it contains the index of the sous-sim using it
    std::vector<int> groups_avail(THREADS, -1); 
    
    // Variable to track the threads and their state
    int threads_used = 0;
    int sous_sim_next_index = 0;
    int sous_sim_total = SOUS_SIM;
    int sous_sim_started = 0;
    int sous_sim_completed = 0;
    
    // Distribute agents among PhysicsWorkers
    std::vector<PhysicsWorker> physicsWorkers;
    for(int i = 0; i < THREADS; i++) {
        physicsWorkers.emplace_back();
    }
    
    // Partition agents for each thread
    std::vector<std::vector<Creature*>> agentPartitions(THREADS);
    for(int i = 0; i < THREADS; i++) {
        auto start = agents.begin() + i * (agents.size() / THREADS);
        auto end = (i == THREADS - 1) ? agents.end() : agents.begin() + (i + 1) * (agents.size() / THREADS);

        for (auto it = start; it != end; ++it) {
            agentPartitions[i].push_back(&(*it));
        }

        logm("Created partition " + std::to_string(i) + " with " + std::to_string(agentPartitions[i].size()) + " agents.");
    }

    for(int i=0; i<sous_sim_total; i++) {
        sous_sim_threads.emplace_back(std::thread());
    }

    // DisplayService init
    bool running = true;
    std::unique_ptr<DisplayService> dpl_srvc = CreateDisplayService();
    dpl_srvc->init(headless, &running, &clean_exit, agentPartitions.size(), mc);

    // Manage Clock
    sf::Clock clock;
    float fps = 0.0f;

    // Generate goals
    std::vector<std::vector<sf::Vector2f>> goals;

    // Random number setup
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    float ran = 0;
    float angle = 0;
    float radius = 0;
    float x = 0;
    float y = 0;

    for(int i=0; i<sous_sim_total; i++) {
        std::vector<sf::Vector2f> temp;

        angle = (2 * M_PI / sous_sim_total) * i;
        radius = (MAXDIST-MINDIST)*dist(gen) + MINDIST;
        x = start.x + radius * cos(angle);
        y = start.y + radius * sin(angle);
        temp.emplace_back(sf::Vector2f(x, y));

        for(int j=1; j<NB_GOAL; j+=1){
            ran = dist(gen)*2*M_PI;  // [0,2*pi[
            radius = (MAXDIST-MINDIST)*dist(gen) + MINDIST;
            x += start.x + radius * cos(ran);
            y += start.y + radius * sin(ran);
            temp.emplace_back(sf::Vector2f(x, y));   
        }
        goals.emplace_back(temp);
        temp.clear();
    }

    // Initialisation of score variables
    for(int i = 0; i < SOUS_SIM; i++) {
        sous_sim_scores[i] = std::vector<float>(nb_brain, 0.0f);
    }
    
    // Init of evolution curve
    if (use_evolution_curve) {
        evolution = curve_a*std::exp(curve_b*generation+curve_c)+curve_d;
    }
    
    logm("All variable initialized. Starting main loop.", "INFO");
    
    // Main loop
    while (running) {

        if (dt < 1e-6f) dt = 1e-6f;
        fps = 1.0f / dt;
        
        // stop program if it take to much time and in client mode
        if (!is_infinite && !mc) {
            if (acc > time_allowed) {
            clean_exit = true;
            }
        }
        
        // Checks for the client mode (mc stand for client mode)
        if(mc) {
            switch (cl->state) {
                case -1:
                    return 1;
                break;

                case 2: continue;
                break;

                case 3: 
                    // End of generation for client mode

                    score_agent = cl->scores;
                    brain_agent.clear();

                    // Initialize agents with brain from file
                    for (int i=0; i<nb_brain; i++) {
                        brain_agent.emplace_back(9,6,cl->sbfpath+cl->selectioned[i],DEVICE,NB_HIDDEN_LAYER);
                    }

                    reproduce(&brain_agent, score_agent,  nb_brain, evolution, BEST_KEEP, SELECTION_POL);
                    
                    for(int j=0;j<nb_brain;j++) score_agent[j] = 0;
                    sous_sim_next_index = 0;
                    sous_sim_started = 0;
                    sous_sim_completed = 0;
                    sous_sim_threads.clear();
                    sous_sim_state.clear();

                    for(int i=0; i<sous_sim_total; i++) {
                        sous_sim_threads.emplace_back(std::thread());
                        sous_sim_state.emplace_back(0);
                    }

                    generation += 1;

                    if (use_evolution_curve) {
                        evolution = curve_a*std::exp(curve_b*generation+curve_c)+curve_d;
                    }
                    
                    acu = 0;
                    cl->state = 1;
                break;

                case 4:
                    clean_exit = true;
                break;

            }
        }

        // Check if any sous-sim thread has finished
        for(int i = 0; i < sous_sim_total; i++) {
            if(sous_sim_state[i] == 2) {
                // Thread has finished
                threads_used -= 1;

                // Search what group it was using
                for (int j = 0; j < groups_avail.size(); j++) {
                    if (groups_avail[j] == i) {
                        groups_avail[j] = -1; // Mark group as available
                        break;
                    }
                }

                sous_sim_state[i] = 3; // Mark as collected
                sous_sim_completed += 1;
            }
        }
        
        // Handle a clean exit of the simulation to avoid segmentation fault
        if(clean_exit) {
            // Wait the end of all threads
            if (threads_used == 0) {
                if (!mc) {
                    for (int i=0; i<nb_brain; i++) {
                        std::string istring = std::to_string(i);
                        brain_agent[i].saveFile(sds.getFullPath()+istring+".pt");
                    }
                    sds.data["generation"] = generation;
                    sds.data["simu_time"] = simu_time;
                    sds.data["evolution"] = evolution;
                    sds.data["total_trained_time"] = acc;
                    sds.save();
                    
                    logm("Saved.", "INFO");
                } 
                
                logm("Exiting simulation.", "INFO");
                return 0;
            }
        }
        
        
        if(sous_sim_started >= sous_sim_total) {
            
            // Waiting for threads to finish
            if (sous_sim_completed >= sous_sim_total) {
                // All of it is finished
                // Change of sim :
                logm("Generation finished. Processing...", "INFO");
                
                for(int i=0;i<sous_sim_scores.size();i++) {
                    for(int j=0;j<nb_brain;j++) {
                        score_agent[j] += sous_sim_scores[i][j];
                    }
                }

                // Normalise the score to be able too compare them
                for(int j=0;j<nb_brain;j++) {
                        if(score_agent[j] != 0) score_agent[j] /= sous_sim_total;
                }
                
                int best_score_index = std::distance(score_agent.begin(), std::max_element(score_agent.begin(), score_agent.end()));
                
                for(int i=0; i<sous_sim_total; i++) {
                    std::vector<sf::Vector2f> temp;

                    angle = (2 * M_PI / sous_sim_total) * i;
                    radius = (MAXDIST-MINDIST)*dist(gen) + MINDIST;
                    x = start.x + radius * cos(angle);
                    y = start.y + radius * sin(angle);
                    temp.emplace_back(sf::Vector2f(x, y));

                    for(int j=1; j<NB_GOAL; j+=1){
                        ran = dist(gen)*2*M_PI;  // [0,2*pi[
                        radius = (MAXDIST-MINDIST)*dist(gen) + MINDIST;
                        x += start.x + radius * cos(ran);
                        y += start.y + radius * sin(ran);
                        temp.emplace_back(sf::Vector2f(x, y));   
                    }
                    goals.emplace_back(temp);
                    temp.clear();
                }


                if (!mc) { // We only want to save brain in classic mode here.
                   
                    float generationTemp = generation;
                    float agent0scoreTemp= score_agent[0];
                    float agent1scoreTemp= score_agent[1];
                    float agent2scoreTemp= score_agent[2];
                    float agent3scoreTemp= score_agent[3];
                    float agent4scoreTemp= score_agent[4];
                    float agent5scoreTemp= score_agent[5];
                    float agent6scoreTemp= score_agent[6];
                    float agent7scoreTemp= score_agent[7];
                    float agent8scoreTemp= score_agent[8];
                    float agent9scoreTemp= score_agent[9];
                    float meanTemp = average(score_agent);
                    float bestAgentScoreTemp = score_agent[best_score_index];
                    float timeForOneGenTemp = acu;

                    // Initialization the variables for the next generation
                    reproduce(&brain_agent, score_agent,  nb_brain, evolution, BEST_KEEP, SELECTION_POL);
                    
                    // Saving data
                    int midid = score_agent.size()/2;
                    float median = score_agent[midid];
                    
                    sds.addStatRow(generationTemp, agent0scoreTemp, agent1scoreTemp, agent2scoreTemp, 
                       agent3scoreTemp, agent4scoreTemp, agent5scoreTemp, agent6scoreTemp,
                       agent7scoreTemp, agent8scoreTemp, agent9scoreTemp, meanTemp, median,
                       bestAgentScoreTemp,timeForOneGenTemp,0,0,std::time(nullptr),0);
                
                    // Autosave check
                    if (AUTOSAVE && generation % AUTOSAVE_FREQ == 0) {
                        // Time to autosave !
                        for (int i=0; i<nb_brain; i++) {
                            std::string istring = std::to_string(i);
                            brain_agent[i].saveFile(sds.getFullPath()+istring+".pt");
                        }
                        sds.data["generation"] = generation;
                        sds.data["simu_time"] = simu_time;
                        sds.data["evolution"] = evolution;
                        sds.data["total_trained_time"] = acc;
                        sds.save();
                        
                        logm("Autosaved.", "INFO");
                    }
                    
                    
                    for(int j=0;j<nb_brain;j++) score_agent[j] = 0;
                    sous_sim_next_index = 0;
                    sous_sim_started = 0;
                    sous_sim_completed = 0;
                    sous_sim_threads.clear();
                    sous_sim_state.clear();

                    for(int i=0; i<sous_sim_total; i++) {
                        sous_sim_threads.emplace_back(std::thread());
                        sous_sim_state.emplace_back(0);
                    }

                    generation += 1;
                    if (use_evolution_curve) {
                        evolution = curve_a*std::exp(curve_b*generation+curve_c)+curve_d;
                    }
                    acu = 0;

                } else { // We are in client mode so we save the brains here
                    
                    for (int i=0; i<nb_brain; i++) {
                        std::string istring = std::to_string(i);
                        brain_agent[i].saveFile(cl->sbfpath+cl->srvid+"s"+istring+".pt");
                    }
                    
                    std::string temptest = vectf_to_jsonstring(score_agent);
                    Packet genf("genfinished",std::to_string(generation),temptest,"");
                    cl->send(genf);
                    cl->state = 2;
                }
            }
        } else {
            
            // Not all sous-sim started yet
            while(threads_used < THREADS) {
                // There is one or more free thread
                
                // Avoid starting sous-sim that don't exist
                if(sous_sim_started >= sous_sim_total) break;

                // find on of the available thread
                int group_index = -1;
                for (int i = 0; i < groups_avail.size(); i++) {
                    if (groups_avail[i] == -1) { // Group is available
                        group_index = i;
                        groups_avail[i] = sous_sim_next_index; // Mark group as used
                        break;
                    }
                }
    
                if (group_index == -1){ 
                    error("No group available. Have you made a mistake ???"); // No free group to work with
                }
    
                // Making a Vector of Brain pointers for the group
                std::vector<Brain*> brain_agent_ptrs;
                for (auto& b : brain_agent) brain_agent_ptrs.push_back(&b);
                
                sous_sim_started+=1;
                logm("Starting sous-sim " + std::to_string(sous_sim_next_index) + " on group " + std::to_string(group_index) + " with " + std::to_string(agentPartitions[group_index].size()) + " agents.", "THREAD");
                    
                sous_sim_threads[sous_sim_next_index] = std::thread(handleThread, &physicsWorkers[group_index], agentPartitions[group_index], start, goals[sous_sim_next_index], brain_agent_ptrs, &sous_sim_state[sous_sim_next_index], &sous_sim_scores[sous_sim_next_index], &ss_dt, simu_time, ss_dt*BRAIN_ACC);
                sous_sim_threads[sous_sim_next_index].detach(); // Detach the thread to make it run on is own
                sous_sim_state[sous_sim_next_index] = 1; // Mark it as running
                threads_used += 1;
                sous_sim_next_index += 1;
            }
        }
        
        //// Window drawing block here
        if (!headless) {
            dpl_srvc->render(&groups_avail, &agentPartitions, fps, agents.size(), generation, sous_sim_started, acu, simu_time, evolution, start, goals[0], mc);
        }
        ////

        acc += dt;
        acu += dt;
        dt = clock.restart().asSeconds();

    }

    logm("Exiting simulation.", "INFO");
    return 0;
}

SimpleClient::SimpleClient(const std::string &uri, const std::string path) {

    sbfpath = path+"/";
    m_client.init_asio();
    m_client.clear_access_channels(websocketpp::log::alevel::all);
    m_client.clear_error_channels(websocketpp::log::elevel::all);
    m_client.set_message_handler([this](websocketpp::connection_hdl, client::message_ptr msg) {
        
        std::string payload = msg->get_payload();
        Packet r(payload);
        
        if (r.cmd == "connected") {
        	srvid = r.arg1;
        }
        
        if (r.cmd == "startsim") {
        	logm("Server requested the start of sim #" + r.arg1,"Client");
        	mstk->loadTask(std::stoi(r.arg1));
        	mstk->sim_name = mstk->sim_name + srvid;
        	state = 1;
        }
        
        if (r.cmd == "nextgen") {
        	logm("Server requested the next gen","Client");
        	selectioned = jsonstring_to_vects(r.arg1);
        	scores = jsonstring_to_vectf(r.arg2);
        	state = 3;
        }
        
        if (r.cmd == "stopsim") {
        	logm("Server requested the end of sim","Client");
        	state = 4;
        }
        
        if (r.cmd == "exit") {
        	logm("Server requested exiting...","Client");
        	state = -1;
        }
    });

    websocketpp::lib::error_code ec;
    client::connection_ptr con = m_client.get_connection(uri, ec);
    if (ec) {
        logm("Could not create connection: ", "ERROR");
        return;
    }

    m_hdl = con->get_handle();
    m_client.connect(con);

    m_thread = std::thread([this]() { m_client.run(); });
}

SimpleClient::~SimpleClient() {
    m_client.stop_perpetual();
    if (m_thread.joinable())
        m_thread.join();
}

void SimpleClient::send(Packet pck) {
    websocketpp::lib::error_code ec;
    m_client.send(m_hdl, pck.get_string(), websocketpp::frame::opcode::text, ec);
    if (ec) {
        logm("Send failed: " + ec.message(),"ERROR");
    }
}

void SimpleClient::run(SimTasker* stk, bool hl) {

    headless = hl;
    mstk = stk;
    
    // Connecting...
    std::this_thread::sleep_for(std::chrono::seconds(1));
    Packet conn("connect",getHostName(),"","");
    send(conn);
    
    while (srvid == "") continue;
    logm("Connected to server with id #" + srvid,"Client");
    
    while (state != -1) {
        if (state==0) {std::this_thread::sleep_for(std::chrono::milliseconds(200));}
        if (state==1) {
            simulate(*mstk, true, headless, this);
            if (state == -1) break;
            state=0;
            Packet simfnsh("simfinished","","","");
            send(simfnsh);
        }
    }

    logm("Shuting down...","Client");
    
    // Close connection with the server
    websocketpp::lib::error_code ec;
    m_client.close(m_hdl, websocketpp::close::status::normal, "Client shutting down", ec);
    if (ec) {
        logm("Close failed: " + ec.message(), "ERROR");
    }
    
    // Stop the ASIO event loop and wait for the thread to end
    // For once...
    m_client.stop();
    if (m_thread.joinable()){
        m_thread.join();
    }
    logm("Shutdown.","Client");
}