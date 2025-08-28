#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <map>
#include <cmath>

#include "physics.h"
#include "creature.h"
#include <iostream>

void log(const std::string& message, const std::string& level = "INFO")
{
    std::cout << "[" << level << "] " << message << std::endl;
}

void drawStats(sf::RenderWindow& window, const sf::Font& font, const std::map<std::string, float>& stats)
{
    // Draw statistics on the window

    sf::Vector2u size = window.getSize();
    float rectangleWidth = 200.f;
    float rectangleHeight = 50.f + stats.size() * 25.f;

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
        sf::Text text(font);
        
        text.setString(key + ": " + std::to_string(value));
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);
        text.setPosition({rectX + margin, rectY + margin + count});
        count += 25.f;
        window.draw(text);
    }
    

    
}

void init_agents_and_brain(int countAgents, int countBrains, int x, int y, std::vector<Creature>* agents, std::vector<std::string>* brains, std::string brainFile="")
{
    if (brainFile == "")
    {
        for(int i=0; i<countAgents; i++)
        {
            agents->emplace_back(x, y, i);

        }
        for (int i=0; i<countBrains; i++)
        {
            brains->emplace_back("");
        }
        // Initialize agents with default brain
    } else {
        // Initialize agents with brain from file
        log("Loading brains from file is not implemented yet.", "WARNING");
    }

    
}

int main()
{

    /// SETTINGS ///
    std::string DEVICE = "cpu"; // "cpu" or "gpu"

    bool LOAD_FROM_FILE = false;
    std::string LOAD_NAME = "test2_20250513_221021";

    std::string SIM_NAME = "test2";

    // SIM //
    int SIM_TIME = 100;
    float EVOLUTION = 0.375;
    int NB_BRAIN = 200;
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
    ///////////////////

    ////// variable de l'état de la simulation ///////
    bool running = true;
    int inputdelayBase = 10;
    int simu_time = SIM_TIME;
    float br_acc = BR_ACC/FACTEUR;
    float evolution = EVOLUTION;
    int generation = 0;
    int acc = 0;
    int acu = 0;
    int cyl = 0;
    int sous_sim = 1;
    float inputdelay = inputdelayBase;
    int selected_agent = 0;
    bool drawall = false;
    std::vector<float> fpsM;

    std::vector<float> score_agent(NB_BRAIN, 0);
    std::vector<float> agents_objectif(NB_AGENT, 0);
    std::vector<std::string> brain_agent;
    std::vector<Creature> agents;
    int cycle = 0;

    if (LOAD_FROM_FILE)
    {
        // Load simulation state from file
    }
    else
    {
        // Initialize simulation state
        init_agents_and_brain(NB_AGENT, NB_BRAIN, 100, 100, &agents, &brain_agent);
    }

    auto window = sf::RenderWindow(sf::VideoMode({1050u, 750u}), "URSAFSIM");
    window.setFramerateLimit(60);

    // Load Textures
    sf::Font font("assets/fonts/arial.ttf");
    sf::Texture backgroundTexture("assets/textures/background.png");

    // Create Sprite
    sf::Sprite backgroundSprite(backgroundTexture);

    // Manage Clock
    sf::Clock clock;
    float fps = 0.0f;

    while (window.isOpen())
    {
        sf::Time frameStart = clock.restart();
        float dt = frameStart.asSeconds();


        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
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

        // On gere la physique des créatures
        // TODO: Implement creature physics

        

        fps = 1.0f / frameStart.asSeconds();


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


        drawStats(window, font, {{"FPS", std::round(fps)}, {"nb_agent", agents.size()}, {"selected", selected_agent}, {"gen", generation},{"sous-gen", sous_sim},{"tps", simu_time}, {"evolution", evolution}});

        window.display();
    }
}
