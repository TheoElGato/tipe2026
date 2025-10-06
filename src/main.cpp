#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <map>
#include <cmath>
#include <thread>
#include <iostream>

#include "physics.h"
#include "creature.h"
#include "brain.h"
#include "threadsmg.h"

/// SETTINGS ///

std::string DEVICE = "cpu"; // "cpu" or "gpu"
int THREADS = 8;

bool LOAD_FROM_FILE = false;
std::string LOAD_NAME = "test2_20250513_221021";

std::string SIM_NAME = "test2";

// SIM //

int SIM_TIME = 100;
float EVOLUTION = 0.375;
int NB_BRAIN = 300;
int NB_AGENT = NB_BRAIN*THREADS;
int NB_HIDDEN_LAYER = 100;
int SOUS_SIM = 20;


// AUTOSAVE //

bool AUTOSAVE = true;
int AUTOSAVE_FREQ = 1;

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

void init_agents_and_brain(int countAgents, int countBrains, int x, int y, std::vector<Creature>* agents, std::vector<Brain>* brains, std::string brainFile="")
{
    if (brainFile == "")
    {
        for(int i=0; i<countAgents; i++)
        {
            agents->emplace_back(x, y, i);

        }
        for (int i=0; i<countBrains; i++)
        {
            brains->emplace_back(9,6,DEVICE,"",NB_HIDDEN_LAYER);
        }
        // Initialize agents with default brain
    } else {
        // Initialize agents with brain from file
        log("Loading brains from file is not implemented yet.", "WARNING");
    }

    
}

void physicsUpdate(PhysicsWorker& physics, std::vector<Creature*> agents, float dt)
{
    for (auto &agent : agents) {
        std::vector<Point>* vertices = &agent->vertices;
        physics.PBD(vertices, agent->links, agent->muscles, 10, dt);
    }
}

int main()
{

    /// temp
    float ss_dt = 1/60.f;
    

    ////// variable related to the state of simulation ///////
    bool running = true;

    // var input handling
    int inputdelayBase = 10;
    float inputdelay = (float)inputdelayBase;
    
    // var simulation gestion
    int simu_time = SIM_TIME;
    float evolution = EVOLUTION;
    int generation = 0;

    // accumulators
    float acc = 0; // unused
    float acu = 0;
    int cyl = 0;

    // selected agent
    int selected_agents = 0;
    bool drawall = false;

    // temp
    float dt = 0.016f;

    // data for agents and brains
    std::vector<float> score_agent(NB_BRAIN, 0);
    std::vector<float> agents_objectif(NB_AGENT, 0);
    std::vector<Brain> brain_agent;
    std::vector<Creature> agents;

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

    if (LOAD_FROM_FILE)
    {
        // Load simulation state from file
        init_agents_and_brain(NB_AGENT, NB_BRAIN, start.x, start.y, &agents, &brain_agent, LOAD_NAME);

    }
    else
    {
        // Initialize simulation state
        init_agents_and_brain(NB_AGENT, NB_BRAIN, start.x, start.y, &agents, &brain_agent);
    }

    
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

    // Create Sprite
    sf::Sprite backgroundSprite(backgroundTexture);

    // Manage Clock
    sf::Clock clock;
    float fps = 0.0f;

    // dummy goal
    sf::Vector2f goal(250, 250);


    // Initialisation of score variables
    for(int i = 0; i < SOUS_SIM; i++) {
        sous_sim_scores[i] = std::vector<float>(NB_BRAIN, 0.0f);
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
                running = false;
                log("Waiting for threads to finish...", "THREAD");
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

                if (!running){
                    window.close();
                    break;
                }

                log("Generation finished. Processing...", "INFO");
                
                for(int i=0;i<sous_sim_scores.size();i++) {
                    for(int j=0;j<NB_BRAIN;j++) {
                        score_agent[j] += sous_sim_scores[i][j];
                    }
                }
                
                for(int j=0;j<NB_BRAIN;j++) {
                        if(score_agent[j] != 0) score_agent[j] /= sous_sim_total;
                }
                
                log(std::to_string(score_agent[0]),"DEBUG");
                log(std::to_string(score_agent[1]),"DEBUG");
                log(std::to_string(score_agent[2]),"DEBUG");

                
                Creature best_agent = agents[std::distance(score_agent.begin(), std::max_element(score_agent.begin(), score_agent.end()))];

                // Sauvegarde automatique
                if (AUTOSAVE && generation % AUTOSAVE_FREQ == 0) {
                    // TODO: implement autosave
                    log("Autosave is not implemented yet.", "WARNING");
                }
                
                // initialization the variables for the next generation
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
    
                
    
                sous_sim_threads[sous_sim_next_index] = std::thread(handleThread, &physicsWorkers[group_index], agentPartitions[group_index], start, goal, brain_agent_ptrs, &sous_sim_state[sous_sim_next_index], &sous_sim_scores[sous_sim_next_index], &ss_dt, simu_time/10, ss_dt*4);
                sous_sim_threads[sous_sim_next_index].detach(); // Détacher le thread pour qu'il s'exécute indépendamment
    
                sous_sim_state[sous_sim_next_index] = 1; // Marquer comme en cours d'exécution
    
                threads_used += 1;
                sous_sim_next_index += 1;
            }
        
        }

        window.clear();
        window.draw(backgroundSprite);

        // Drawing of the agents
        if (drawall) {
            for ( Creature* agent : agentPartitions[selected_agents]) {
                agent->draw(window);
            }
        }

        drawStats(window, font, {{"FPS", std::round(fps)}, {"Nb agents", agents.size()}, {"SGen Selected", selected_agents}, {"Current Gen", generation},{"Current SGen", sous_sim_started},{"Tps",round(acu)},{"Tps max", simu_time}, {"Evolution", evolution}});

        window.display();

        //acc += dt;
        acu += dt;
        dt = clock.restart().asSeconds();

    }

    log("Exiting simulation.", "INFO");

    return 0;
}
