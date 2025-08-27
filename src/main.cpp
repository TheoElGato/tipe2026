#include <SFML/Graphics.hpp>
#include <map>
#include <cmath>

#include "physics.h"

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

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1050u, 750u}), "URSAFSIM");
    window.setFramerateLimit(1440);

    // Load Textures
    sf::Font font("assets/fonts/arial.ttf");
    sf::Texture backgroundTexture("assets/textures/background.png");

    // Create Sprite
    sf::Sprite backgroundSprite(backgroundTexture);

    // Manage Clock
    sf::Clock clock;
    float fps = 0.0f;

    // TEEST OF PHYSICS
    Point p1(0.0f, 0.0f, 1.0f);

    while (window.isOpen())
    {
        sf::Time frameStart = clock.restart();


        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        fps = 1.0f / frameStart.asSeconds();


        window.clear();

        window.draw(backgroundSprite);
        drawStats(window, font, {{"FPS", std::round(fps)}, {"nb_agent", 0.0f}, {"selected", 0.0f}, {"gen", 0.0f}, {"tps", 0.0f}, {"evolution", 0.0f}});

        window.display();
    }
}
