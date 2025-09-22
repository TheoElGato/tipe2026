#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <map>
#include <cmath>
#include <thread>
#include <iostream>

#include "physics.h"
#include "creature.h"
#include "brain.h"

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
int FACTEUR = 4;
int NB_AGENT = NB_BRAIN*FACTEUR;
float BR_ACC = 0.5;
int NB_HIDDEN_LAYER = 100;
int SOUS_SIM = 40/FACTEUR;

//SCORE_FUNCTION = reproduction.score_distance

// AUTOSAVE //
bool AUTOSAVE = true;
int AUTOSAVE_FREQ = 1;

// GOAL //
int MAXDIST = 300;
int MINDIST = 200;
int NB_GOAL = 3;

//// Position de depart
sf::Vector2f start = sf::Vector2f(523, 375);
///////////////////





void log(const std::string& message, const std::string& level = "INFO")
{
    std::cout << "[" << level << "] " << message << std::endl;
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

    

    ////// variable de l'état de la simulation ///////
    bool running = true;
    int inputdelayBase = 10;
    int simu_time = SIM_TIME;
    float br_acc = BR_ACC/FACTEUR;
    float evolution = EVOLUTION;
    int generation = 0;
    float acc = 0;
    float acu = 0;
    int cyl = 0;
    int sous_sim = 1;
    float inputdelay = inputdelayBase;
    int selected_agent = 0;
    bool drawall = false;
    std::vector<float> fpsM;
    float dt = 0.016f;

    std::vector<float> score_agent(NB_BRAIN, 0);
    std::vector<float> agents_objectif(NB_AGENT, 0);
    std::vector<Brain> brain_agent;
    std::vector<Creature> agents;
    int cycle = 0;

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

    
    // Repartition des agents dans leur PhysicsWorker
    std::vector<PhysicsWorker> physicsWorkers;
    for(int i = 0; i < THREADS; i++) {
        physicsWorkers.emplace_back();
    }
    
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

    sf::Vector2f goal(250, 250);

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
                selected_agent = (selected_agent - 1) % agents.size();
                inputdelay = 0;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
            {
                selected_agent = (selected_agent + 1) % agents.size();
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

        // Physics update
        std::vector<std::thread> threads;
        for (size_t i = 0; i < physicsWorkers.size(); ++i) {
            threads.emplace_back(physicsUpdate, std::ref(physicsWorkers[i]), agentPartitions[i], dt);
        }

        for (auto& thread : threads) {
            thread.join();
        }
        // Delete threads
        threads.clear();

        // Ticking Brain
        if (br_acc <= acc) {

            if (cycle>=FACTEUR) {
                cycle=0;
            }

            for(int i=NB_BRAIN*cycle;i<NB_BRAIN*(cycle+1);i++) {
                agents[i].brainUpdate(goal,&brain_agent[i%NB_BRAIN]);
                agents[i].update(dt);
            }

            cycle += 1;
            acc = 0;

        }

        // Changement de sous sim / sim:
        if (acu >= simu_time) {

            // Changement de sous sim :
            if (sous_sim < SOUS_SIM) {

                // Calcul des scores de chaque agent
                // Et renitialisation de leur position
                for(int cyc=0;cyc<FACTEUR;cyc++) {
                    for (int i = NB_BRAIN * cyc; i < NB_BRAIN * (cyc + 1); i++) {
                        score_agent[i % NB_BRAIN] += score_distance(&agents[i], goal);
                        agents[i].moveTo(start.x, start.y);
                    }
                }

                sous_sim += 1;


            } else {
                // Changement de sim :
                Creature best_agent = agents[std::distance(score_agent.begin(), std::max_element(score_agent.begin(), score_agent.end()))];
                float FPSm = 0.0f;
                for (const auto& fps : fpsM) {
                        FPSm += fps;
                }
                if (FPSm > 0.0f) {
                    FPSm /= fpsM.size();
                }

                // Sauvegarde automatique
                if (AUTOSAVE && generation % AUTOSAVE_FREQ == 0) {
                    // TODO: implement autosave
                    log("Autosave is not implemented yet.", "WARNING");
                }
            }

            acu = 0;

        }




        window.clear();
        window.draw(backgroundSprite);

        // Affichage créatures
        if (drawall) {
            for ( auto& agent : agents) {
                agent.draw(window);
            }
        } else {
            agents[selected_agent].draw(window);
        }

        drawStats(window, font, {{"FPS", std::round(fps)}, {"nb_agent", agents.size()}, {"selected", selected_agent}, {"gen", generation},{"sous_gen", sous_sim},{"tps",round(acu)},{"tps_max", simu_time}, {"evolution", evolution}});

        window.display();

        acc += dt;
        acu += dt;
        dt = clock.restart().asSeconds();

    }
}
