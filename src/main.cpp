#include <SFML/Graphics.hpp>

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1050u, 750u}), "URSAFSIM");
    window.setFramerateLimit(144);

    // Load Textures
    sf::Texture backgroundTexture("assets/textures/background.png");

    // Create Sprite
    sf::Sprite backgroundSprite(backgroundTexture);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }


        window.clear();
        
        window.draw(backgroundSprite);

        window.display();
    }
}
