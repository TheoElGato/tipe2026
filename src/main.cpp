#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <map>
#include <cmath>
#include <thread>
#include <iostream>

#include "reproduction.h"
#include "physics.h"
#include "creature.h"
#include "brain.h"
#include "threadsmg.h"
#include "filemanager.h"

/// SETTINGS ///

std::string DEVICE = "cpu"; // "cpu" or "gpu"
int THREADS = 8;

bool LOAD_FROM_FILE = true;
std::string LOAD_NAME = "test2_20251014_213011";

std::string SIM_NAME = "test2";

// SIM //

int SIM_TIME = 10;
float EVOLUTION = 0.075;
int NB_BRAIN = 300;
int NB_AGENT = NB_BRAIN*THREADS;
int NB_HIDDEN_LAYER = 100;
int SOUS_SIM = 32;
int BEST_KEEP = 20;
int SELECTION_POL = 50;


// AUTOSAVE //

bool AUTOSAVE = true;
int AUTOSAVE_FREQ = 4;

// GOAL //

int MAXDIST = 300;
int MINDIST = 200;
int NB_GOAL = 3;

//// Start position
sf::Vector2f start = sf::Vector2f(523, 375);
///////////////////





void log(const std::string& message, const std::string& level = "INFO")
{   
    // Standardized logging function
    std::cout << "[" << level << "] " << message << std::endl;
}

void error(const std::string& message)
{
    // Standardized error function
    log(message, "ERROR");
    exit(EXIT_FAILURE);
}

std::string nozero(float value) {
    std::ostringstream oss;
    oss << std::setprecision(8) << std::noshowpoint << value;
    return oss.str();
}

void drawStats(sf::RenderWindow& window, const sf::Font& font, const std::map<std::string, float>& stats)
{
    // Draw statistics on the window

    sf::Vector2u size = window.getSize();
    float rectangleWidth = 200.f;
    float rectangleHeight = 50.f + stats.size() * 20.f;

    float margin = 10.f;
    float rectX = size.x - rectangleWidth - margin;
    float rectY = 0 + margin;

    sf::RectangleShape rectangle({rectangleWidth, rectangleHeight});
    rectangle.setFillColor(sf::Color(200,200,200));
    rectangle.setPosition({rectX, rectY});
    window.draw(rectangle);

    float count = 0;
    for (const auto& [key, value] : stats)
    {
        // Create a text object for each statistic
        sf::Text text;
        text.setFont(font);

        text.setString(key + ": " + nozero(value));
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);
        text.setPosition({rectX + margin, rectY + margin + count});
        count += 25.f;
        window.draw(text);
    }
}

void draw_items(sf::RenderWindow& window, sf::Sprite sp, sf::Sprite ob, std::vector<sf::Vector2f> g, int idsssim) {
    if (idsssim < 0) idsssim = 0;
    sp.scale({0.6f, 0.6f});
    ob.scale({0.6f, 0.6f});
    sf::FloatRect spb = sp.getLocalBounds();
    sf::FloatRect obb = ob.getLocalBounds();
    sp.setOrigin(spb.width/2, spb.height/2);
    ob.setOrigin(obb.width/2, obb.height/2);
    sp.setPosition(start);
    ob.setPosition(g[idsssim]);
    window.draw(sp);
    window.draw(ob);
}

void init_agents_and_brain(int countAgents, int countBrains, int x, int y, std::vector<Creature>* agents, std::vector<Brain>* brains, std::string brainFile="")
{
    for(int i=0; i<countAgents; i++)
    {
        agents->emplace_back(x, y, i);

    }
    if (brainFile == "")
    {
        // Initialize agents with default brain
        for (int i=0; i<countBrains; i++)
        {
            brains->emplace_back(9,6,"",DEVICE,NB_HIDDEN_LAYER);
        }
    } else {
        // Initialize agents with brain from file
        for (int i=0; i<countBrains; i++)
        {
            std::string istring = std::to_string(i);
            log("Loading brain"+istring+" from file.");
            brains->emplace_back(9,6,brainFile+istring+".pt",DEVICE,NB_HIDDEN_LAYER);
        }
    }

    
}

// inutiliser
void physicsUpdate(PhysicsWorker& physics, std::vector<Creature*> agents, float dt)
{
    for (auto &agent : agents) {
        std::vector<Point>* vertices = &agent->vertices;
        physics.PBD(vertices, agent->links, agent->muscles, 10, dt);
    }
}

int main()
{
    log("Welcome to the USRAF Sim");

    /// temp
    float ss_dt = 1/60.f;
    

    ////// variable related to the state of simulation ///////

    // var input handling
    int inputdelayBase = 10;
    float inputdelay = (float)inputdelayBase;
    
    // var simulation gestion
    int nb_agent = NB_AGENT;
    int nb_brain = NB_BRAIN;
    int simu_time = SIM_TIME;
    float evolution = EVOLUTION;
    int generation = 0;

    // accumulators
    float acc = 0;
    float acu = 0;
    int cyl = 0;

    // selected agent
    int selected_agents = 0;
    bool drawall = false;

    // temp
    float dt = 0.016f;
    
    
    // Some necessery data for brains and agents
    std::vector<Brain> brain_agent;
    std::vector<Creature> agents;
    
    // Init the SimDataStruct
    std::string sim_name = SIM_NAME;
    if (LOAD_FROM_FILE) sim_name=LOAD_NAME;
    SimDataStruct sds("save",sim_name,0,0,evolution,nb_brain,nb_agent,1);
    
    // If needed, load the sim
    if (LOAD_FROM_FILE) {
        // Load simulation state from file
        log("Loading the sim","WARNING");
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
        
        init_agents_and_brain(nb_agent, nb_brain, start.x, start.y, &agents, &brain_agent, "save/"+LOAD_NAME+"/");
    }
    else {
        // Initialize simulation state
        init_agents_and_brain(nb_agent, nb_brain, start.x, start.y, &agents, &brain_agent);
    }
    sds.save();

    // data for agents and brains
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
    
    // variable to track the threads and their state
    int threads_used = 0;
    int sous_sim_next_index = 0;
    int sous_sim_total = SOUS_SIM;
    int sous_sim_started = 0;
    int sous_sim_completed = 0;
    
    // For stats time 
    float last_acc = acc;
    
    // Distribute agents among PhysicsWorkers
    std::vector<PhysicsWorker> physicsWorkers;
    for(int i = 0; i < THREADS; i++) {
        physicsWorkers.emplace_back();
    }
    
    // Partition agents for each thread
    std::vector<std::vector<Creature*>> agentPartitions(THREADS);
    for(int i = 0; i < THREADS; i++)
    {
        auto start = agents.begin() + i * (agents.size() / THREADS);
        auto end = (i == THREADS - 1) ? agents.end() : agents.begin() + (i + 1) * (agents.size() / THREADS);
        for (auto it = start; it != end; ++it) {
            agentPartitions[i].push_back(&(*it));
        }
        log("Created partition " + std::to_string(i) + " with " + std::to_string(agentPartitions[i].size()) + " agents.");
    }

    for(int i=0; i<sous_sim_total; i++) {
        sous_sim_threads.emplace_back(std::thread());
    }

    auto window = sf::RenderWindow(sf::VideoMode({1050u, 750u}), "URSAFSIM");
    window.setFramerateLimit(60);

    // Load Textures
    sf::Font font;
    font.loadFromFile("assets/fonts/arial.ttf");
    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("assets/textures/background.png");
    
    // Item textures
    sf::Texture startTexture;
    startTexture.loadFromFile("assets/textures/start.png");
    sf::Texture diamondTexture;
    diamondTexture.loadFromFile("assets/items/diamond.png");
    sf::Texture emeraldTexture;
    emeraldTexture.loadFromFile("assets/items/emerald.png");
    sf::Texture nether_starTexture;
    nether_starTexture.loadFromFile("assets/items/nether_star.png");
    sf::Texture netherite_ingotTexture;
    netherite_ingotTexture.loadFromFile("assets/items/netherite_ingot.png");
    sf::Texture music_disc_othersideTexture;
    music_disc_othersideTexture.loadFromFile("assets/items/zzmusic_disc_otherside.png");

    // Create Sprite
    sf::Sprite backgroundSprite(backgroundTexture);
    sf::Sprite startSprite(startTexture);
    
    sf::Sprite diamondSprite(diamondTexture);
    sf::Sprite emeraldSprite(emeraldTexture);
    sf::Sprite nether_starSprite(nether_starTexture);
    sf::Sprite netherite_ingotSprite(netherite_ingotTexture);
    sf::Sprite music_disc_othersideSprite(music_disc_othersideTexture);
    

    // Manage Clock
    sf::Clock clock;
    float fps = 0.0f;

    // dummy goal
    sf::Vector2f goal(250, 250);

    std::vector<sf::Vector2f> goals;

    for(int i=0; i<sous_sim_total; i++) {
        float angle = (2 * M_PI / sous_sim_total) * i;
        float radius = MINDIST;
        float x = start.x + radius * cos(angle);
        float y = start.y + radius * sin(angle);
        goals.emplace_back(sf::Vector2f(x, y));
    }


    // Initialisation of score variables
    for(int i = 0; i < SOUS_SIM; i++) {
        sous_sim_scores[i] = std::vector<float>(nb_brain, 0.0f);
    }
    
    log("All variable initialized. Starting main loop.", "INFO");

    while (window.isOpen())
    {
        //sf::Time frameStart = clock.restart();
        if (dt < 1e-6f) dt = 1e-6f;
        fps = 1.0f / dt;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        // Gestion des touches :
        if (inputdelay == inputdelayBase) {

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            {
                selected_agents = (selected_agents - 1) % agentPartitions.size();
                inputdelay = 0;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
            {
                selected_agents = (selected_agents + 1) % agentPartitions.size();
                inputdelay = 0;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F))
            {
                drawall = !drawall;
                inputdelay = 0;
            }

        } else {
            inputdelay +=1;
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
                        //log("Sous-sim " + std::to_string(i) + " finished with group " + std::to_string(j), "THREAD");
                        break;
                    }
                }

                sous_sim_state[i] = 3; // Mark as collected
                sous_sim_completed += 1;
                
            }
        }
        
        
        if(sous_sim_started >= sous_sim_total) {
            
            // Waiting for threads to finish
            
            if (sous_sim_completed >= sous_sim_total) {
                // All of it is finished
                // Changement de sim :


                log("Generation finished. Processing...", "INFO");
                
                for(int i=0;i<sous_sim_scores.size();i++) {
                    for(int j=0;j<nb_brain;j++) {
                        score_agent[j] += sous_sim_scores[i][j];
                    }
                }
                
                for(int j=0;j<nb_brain;j++) {
                        if(score_agent[j] != 0) score_agent[j] /= sous_sim_total;
                }
                
                int best_score_index = std::distance(score_agent.begin(), std::max_element(score_agent.begin(), score_agent.end()));
                
                sds.addStatRow(generation, score_agent[0], score_agent[1], score_agent[2], 
                   score_agent[3], score_agent[4], score_agent[5], score_agent[6],
                   score_agent[7], score_agent[8], score_agent[9],
                   score_agent[best_score_index], acc-last_acc);
                last_acc = acc;
                
                // Sauvegarde automatique
                if (AUTOSAVE && generation % AUTOSAVE_FREQ == 0) {
                    // Time to autosave !
                    
                    for (int i=0; i<nb_brain; i++)
                    {
                        std::string istring = std::to_string(i);
                        brain_agent[i].saveFile(sds.getFullPath()+istring+".pt");
                    }
                    
                    sds.data["generation"] = generation;
                    sds.data["simu_time"] = simu_time;
                    sds.data["evolution"] = evolution;
                    sds.data["total_trained_time"] = acc;
                    sds.save();
                    
                    log("Autosaved.", "INFO");
                }
                
                // Initialization the variables for the next generation
                reproduce(&brain_agent, score_agent,  nb_brain, evolution, BEST_KEEP, SELECTION_POL);

                sous_sim_next_index = 0;
                sous_sim_started = 0;
                sous_sim_completed = 0;
                sous_sim_threads.clear();
                sous_sim_state.clear();
                for(int i=0; i<sous_sim_total; i++) {
                    sous_sim_threads.emplace_back(std::thread());
                    sous_sim_state.emplace_back(0);
                }
                
                acu = 0;
                generation += 1;
                

                
            }
        } else {
            
            // Not all sous-sim started yet
            while(threads_used < THREADS) {
                // There is one or more free thread
                
                // Avoid starting sous-sim that don't exist
                if(sous_sim_started >= sous_sim_total) break;

                int group_index = -1;
                for (int i = 0; i < groups_avail.size(); i++) {
                    if (groups_avail[i] == -1) { // Group is available
                        group_index = i;
                        groups_avail[i] = sous_sim_next_index; // Mark group as used
                        break;
                    }
                }
    
                if (group_index == -1){ 
                    error("No group available. Have you made a mistake ???"); // Pas de groupe libre
                }
    
                // making a Vector of Brain pointers for the group
                std::vector<Brain*> brain_agent_ptrs;
                for (auto& b : brain_agent) brain_agent_ptrs.push_back(&b);
    
                
                sous_sim_started+=1;
                log("Starting sous-sim " + std::to_string(sous_sim_next_index) + " on group " + std::to_string(group_index) + " with " + std::to_string(agentPartitions[group_index].size()) + " agents.", "THREAD");
                //log("threads used: " + std::to_string(threads_used) + "/" + std::to_string(THREADS), "THREAD");
    
                sous_sim_threads[sous_sim_next_index] = std::thread(handleThread, &physicsWorkers[group_index], agentPartitions[group_index], start, goals[sous_sim_next_index], brain_agent_ptrs, &sous_sim_state[sous_sim_next_index], &sous_sim_scores[sous_sim_next_index], &ss_dt, simu_time, ss_dt*4);
                sous_sim_threads[sous_sim_next_index].detach(); // Détacher le thread pour qu'il s'exécute indépendamment
    
                sous_sim_state[sous_sim_next_index] = 1; // Marquer comme en cours d'exécution
    
                threads_used += 1;
                sous_sim_next_index += 1;
            }
        
        }

        window.clear();
        window.draw(backgroundSprite);
        draw_items(window,startSprite,diamondSprite,goals,groups_avail[selected_agents]);
        // Drawing of the agents
        if (drawall) {
            for ( Creature* agent : agentPartitions[selected_agents]) {
                agent->draw(window);
            }
        }

        drawStats(window, font, {{"FPS", std::round(fps)}, {"Nb agents", agents.size()}, {"SGen Selected", groups_avail[selected_agents]}, {"Current Gen", generation},{"Current SGen", sous_sim_started},{"Tps",round(acu)},{"Tps max", simu_time}, {"Evolution", evolution}});

        window.display();

        acc += dt;
        acu += dt;
        dt = clock.restart().asSeconds();

    }

    log("Exiting simulation.", "INFO");
    return 0;
}
