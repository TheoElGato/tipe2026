#ifndef CREATURE_H
#define CREATURE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <tuple>

#include "physics.h"
#include "brain.h"


class Creature {
public:
    Creature(float sx, float sy, int bodyColorSeed=0, std::string brainFile="");

    float body_mass;
    float paw_mass;
    float muscle_strength;
    std::vector<float> muscle_con;
    std::vector<float> muscle_len;
    std::vector<float> leg_len;
    float struc_len;

    std::vector<Point> vertices;
    std::vector<Link> links;
    std::vector<Spring> muscles;
    float dir;
    std::vector<float> leg_up;
    std::vector<sf::Color> colors;



    void brainUpdate(sf::Vector2f goal, Brain* brain);
    void update(float dt);
    void moveTo(float x, float y);
    void draw(sf::RenderWindow& window);



private:
    sf::Color unique_color_from_single_number(int number);
};


float score_distance(Creature* agent, sf::Vector2f goal);

#endif // CREATURE_H