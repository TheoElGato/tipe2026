#include "displayservice.hpp"
#include "creature.hpp" // we need Creature definition for drawing
#include <SFML/Graphics.hpp>
#include <cmath>
#include <memory>
#include <iostream>
#include <map>
#include <sstream>
#include <iomanip>

// Concrete SFML-based DisplayService
class DisplayServiceSFML : public DisplayService {
public:
    DisplayServiceSFML() = default;
    ~DisplayServiceSFML() override = default;

    void init(bool headless, bool* runningptr, bool* clean_exitptr, int agentPartitionsSize, bool mc) override {
        // If headless was requested at runtime, we can early-return.
        if (headless) {
            // no window created
            logm("Headless mode was selected");
            this->is_headless = true;
            return;
        }

        this->runningptr = runningptr;
        this->clean_exitptr = clean_exitptr;
        this->mc = mc;
        this->window.create(sf::VideoMode({1050u, 750u}), "URSAFSIM");
        this->window.setFramerateLimit(60);

        inputdelayBase = 10;
        inputdelay = (float)inputdelayBase;
        selected_agents = 0;
        drawall = false;
        agent_partitions_size = agentPartitionsSize;

        // Load assets (paths are relative to executable CWD)
        font.loadFromFile("assets/fonts/arial.ttf");
        backgroundTexture.loadFromFile("assets/textures/background.png");
        wifiTexture.loadFromFile("assets/textures/wifi.png");

        startTexture.loadFromFile("assets/textures/start.png");
        diamondTexture.loadFromFile("assets/items/diamond.png");
        emeraldTexture.loadFromFile("assets/items/emerald.png");
        nether_starTexture.loadFromFile("assets/items/nether_star.png");
        netherite_ingotTexture.loadFromFile("assets/items/netherite_ingot.png");
        music_disc_othersideTexture.loadFromFile("assets/items/zzmusic_disc_otherside.png");

        backgroundSprite.setTexture(backgroundTexture);
        wifiSprite.setTexture(wifiTexture);
        startSprite.setTexture(startTexture);
        diamondSprite.setTexture(diamondTexture);
        emeraldSprite.setTexture(emeraldTexture);
        nether_starSprite.setTexture(nether_starTexture);
        netherite_ingotSprite.setTexture(netherite_ingotTexture);
        music_disc_othersideSprite.setTexture(music_disc_othersideTexture);

        wifiSprite.setPosition({0, 0});
        wifiSprite.scale({0.1f, 0.1f});
    }

    void render(std::vector<int>* groups_avail,
                std::vector<std::vector<Creature*>>* agentPartitions,
                float fps, int agent_size, int generation, int sous_sim_started, float acu,
                float simu_time, float evolution, sf::Vector2f start, const std::vector<sf::Vector2f>& goals, bool mc) override
    {
        if (is_headless) return;

        // process events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // keyboard management (rate limited)
        if (inputdelay == inputdelayBase) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
                selected_agents = (selected_agents - 1 + agent_partitions_size) % agent_partitions_size;
                inputdelay = 0;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) {
                selected_agents = (selected_agents + 1) % agent_partitions_size;
                inputdelay = 0;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F)) {
                drawall = !drawall;
                inputdelay = 0;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::K)) {
                if (mc) {
                    logm("You should not do that !","WARNING");
                } else {
                    *clean_exitptr = true;
                    logm("Waiting for threads to finish before exiting...");
                }
                inputdelay = 0;
            }
        } else {
            inputdelay += 1;
        }

        window.clear();
        window.draw(backgroundSprite);

        // Draw start and item sprites
        draw_items(window, startSprite, diamondSprite, start, std::vector<sf::Vector2f>(), (*groups_avail)[selected_agents]);

        if (drawall) {
            for (Creature* agent : (*agentPartitions)[selected_agents]) {
                draw_creature(agent);
            }
        }

        drawStats(window, font, {{"FPS", std::round(fps)}, {"Nb agents", agent_size}, {"SGen Selected", (*groups_avail)[selected_agents]}, {"Current Gen", generation}, {"SGen started", sous_sim_started}});
        if (mc) window.draw(wifiSprite);

        window.display();
        *runningptr = window.isOpen();
    }
    
    void draw_creature(Creature* creature) override {
    	// This is highly unoptimised, but should work for now.
        std::vector<sf::Color> colors = {
	        sf::Color(0, 0, 255),
	        sf::Color(0, 255, 255),
	        sf::Color(255, 0, 255),
	        sf::Color(34, 0, 25),
	        sf::Color((50*creature->bodyColorSeed+96)%255, (69*creature->bodyColorSeed+24)%255, (42*creature->bodyColorSeed+93)%255)
	    };
	    // Drawing of legs and body-leg lines
	    sf::CircleShape pawUp;
	    sf::CircleShape pawDown;
	    sf::CircleShape body;
	    sf::CircleShape eyeWhite;
	    sf::CircleShape eyeBlack;
	    pawUp = sf::CircleShape(5);
	    pawDown = sf::CircleShape(10);
	    body = sf::CircleShape(20);
	    eyeWhite = sf::CircleShape(8);
	    eyeBlack = sf::CircleShape(2);
	    
	    pawUp.setOrigin({5, 5});
	    pawDown.setOrigin({10, 10});
	    body.setFillColor(colors[4]);
	    body.setOrigin({20, 20});
	    eyeWhite.setFillColor(sf::Color::White);
	    eyeWhite.setOrigin({8, 8});
	    eyeBlack.setFillColor(sf::Color::Black);
	    eyeBlack.setOrigin({2, 2});
	    
	    for (int i = 0; i < 4; ++i) {
	        sf::CircleShape* paw = creature->leg_up[i] ? &pawUp : &pawDown;
	        paw->setFillColor(colors[i]);
	        paw->setPosition(creature->vertices[i+1].position);
	        window.draw(*paw);
	
	        // Line between the body and the leg
	        std::array line =
	        {
	            sf::Vertex{creature->vertices[0].position, colors[i]},
	            sf::Vertex{creature->vertices[i+1].position, colors[i]}
	        };
	        window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
	    }
	
	    // Drawing of muscles (simple lines)
	    for (int i = 0; i < 2; ++i) {
	        std::array line =
	        {
	            sf::Vertex{creature->vertices[i*2+1].position, colors[i]},
	            sf::Vertex{creature->vertices[i*2+2].position, colors[i]}
	        };
	        window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
	    }
	
	    // Body drawing
	    
	    body.setPosition(creature->vertices[0].position);
	    window.draw(body);
	
	    // Drawing eyes (reusing shapes)
	    float eye_offset1 = creature->dir - 0.3f;
	    float eye_offset2 = creature->dir + 0.3f;
	    sf::Vector2f center = creature->vertices[0].position;
	    sf::Vector2f eye1 = center + sf::Vector2f(20 * std::cos(eye_offset1), 20 * std::sin(eye_offset1));
	    sf::Vector2f eye2 = center + sf::Vector2f(20 * std::cos(eye_offset2), 20 * std::sin(eye_offset2));
	
	    
	    eyeWhite.setPosition(eye1);
	    window.draw(eyeWhite);
	    eyeWhite.setPosition(eye2);
	    window.draw(eyeWhite);
	
	    
	    eyeBlack.setPosition(eye1);
	    window.draw(eyeBlack);
	    eyeBlack.setPosition(eye2);
	    window.draw(eyeBlack);
    }

private:

    void drawStats(sf::RenderWindow& window, const sf::Font& font, const std::map<std::string, float>& stats) {
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
        for (const auto& [key, value] : stats) {
            std::ostringstream oss;
            oss << std::setprecision(8) << std::noshowpoint << value;
            sf::Text text;
            text.setFont(font);
            text.setString(key + ": " + oss.str());
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::Black);
            text.setPosition({rectX + margin, rectY + margin + count});
            count += 25.f;
            window.draw(text);
        }
    }

    void draw_items(sf::RenderWindow& window, sf::Sprite sp, sf::Sprite ob, sf::Vector2f /*start*/, std::vector<sf::Vector2f> /*g*/, int idsssim) {
        if (idsssim < 0) idsssim = 0;
        sp.scale({0.6f, 0.6f});
        ob.scale({0.6f, 0.6f});
        sf::FloatRect spb = sp.getLocalBounds();
        sf::FloatRect obb = ob.getLocalBounds();
        sp.setOrigin(spb.width/2, spb.height/2);
        ob.setOrigin(obb.width/2, obb.height/2);
        sp.setPosition({0,0});
        ob.setPosition({100,100});
        window.draw(sp);
        window.draw(ob);
    }

    // SFML objects
    sf::RenderWindow window;
    sf::Font font;
    sf::Texture backgroundTexture, wifiTexture;
    sf::Texture startTexture, diamondTexture, emeraldTexture, nether_starTexture, netherite_ingotTexture, music_disc_othersideTexture;
    sf::Sprite backgroundSprite, wifiSprite, startSprite, diamondSprite, emeraldSprite, nether_starSprite, netherite_ingotSprite, music_disc_othersideSprite;

    // state
    bool mc = false;
    bool is_headless = false;
    bool* runningptr = nullptr;
    bool* clean_exitptr = nullptr;
    int inputdelayBase = 10;
    float inputdelay = 0;
    int selected_agents = 0;
    bool drawall = false;
    int agent_partitions_size = 0;
};

std::unique_ptr<DisplayService> CreateDisplayService() {
    return std::make_unique<DisplayServiceSFML>();
}