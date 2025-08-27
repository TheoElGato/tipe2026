#ifndef CREATURE_H
#define CREATURE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <tuple>

#include "physics.h"

class Creature {
public:
    Creature(float sx, float sy, int bodyColorSeed=0, std::string brainFile="");

    float body_mass;
    float paw_mass;
    float muscle_strength;
    std::tuple<float, float> muscle_con;
    std::tuple<float, float> muscle_len;
    std::tuple<float, float> leg_len;
    float struc_len;

    std::vector<Point> vertices;
    std::vector<Link> links;
    std::vector<Spring> muscles;
    float dir;
    std::vector<bool> leg_up;
    std::vector<sf::Color> colors;




    void update(float dt);
    void draw(sf::RenderWindow& window);



private:
    sf::Color unique_color_from_single_number(int number);
};

#endif // CREATURE_H