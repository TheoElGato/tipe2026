/*
    creature.cpp

    Description:
        DESCRIPTION

    Author:
        R. Benichou
        A. Spadone
*/

#include "creature.hpp"

float score_distance(Creature* agent, sf::Vector2f goal) {
    return (std::sqrt(std::pow(agent->vertices[0].position.x - goal.x, 2) + std::pow(agent->vertices[0].position.y - goal.y, 2)));
}

Creature::Creature(float sx, float sy, int bodyColorSeed, std::string brainFile)
{
    /// Initialization of creature parameters


    // Mass of the creature's elements
    this->body_mass = 10.0f;
    this->paw_mass = 2.0f;

    // Initialization of positions and vectors
    // Length of the creature's limbs
    this->muscle_strength = 100.0f;
    this->muscle_con = {0.0f, 0.0f};
    this->muscle_len = {50.0f, 37.5f};
    this->leg_len = {35.35f, 31.25f};
    this->struc_len = 50.0f;

    // Simple diagrams of the creature
    // Body, paws 1-4
    // leg pattern
    // 2     1
    // | \ / |
    // |  0  |
    // | / \ |
    // 4     3
    this->vertices = {
        Point(sx, sy, this->body_mass),                                        // 0 body center
        Point(sx + this->struc_len/2, sy - this->struc_len/2, this->paw_mass), // 1 right front leg
        Point(sx - this->struc_len/2, sy - this->struc_len/2, this->paw_mass), // 2 left front leg
        Point(sx + this->struc_len/2, sy + this->struc_len/2, this->paw_mass), // 3 right rear leg
        Point(sx - this->struc_len/2, sy + this->struc_len/2, this->paw_mass)  // 4 left rear leg
    };

    // Table showing the links between points and their lengths
    this->links = {
        Link(0, 1, this->leg_len[0]),
        Link(0, 2, this->leg_len[0]),
        Link(0, 3, this->leg_len[0]),
        Link(0, 4, this->leg_len[0]),
        Link(1, 3, this->struc_len),
        Link(2, 4, this->struc_len)
    };

    // Table of the muscles represented as spring
    this->muscles = {
        Spring(1, 2, this->struc_len, this->muscle_strength),
        Spring(3, 4, this->struc_len, this->muscle_strength)
    };

    this->dir = 0.0f;
    this->leg_up = {0, 0, 0, 0};
    this->bodyColorSeed = bodyColorSeed;
}


/*
 * Update the information of the creature with the provided brain.
 * @param target The objective of the creature
 * @param brain The crature brain
 */
void Creature::brainUpdate(sf::Vector2f target, Brain * brain) {
    // Get the input for the brain


    // Get the distance to the target
    sf::Vector2f to_target = target - this->vertices[0].position;
    float to_target_length = std::sqrt(to_target.x * to_target.x + to_target.y * to_target.y);

    // Normalise the vector to the target for the dot product
    if (to_target_length != 0.0f) {
        to_target /= to_target_length;
    }

    // Calculate the direction of the creature as a vector
    sf::Vector2f dire = sf::Vector2f(std::cos(this->dir), std::sin(this->dir));

    float ddx = dire.x * to_target.x + dire.y * to_target.y;
    float ddy = -dire.y * to_target.x + dire.x * to_target.y; // The minus and the inversion of x and y is for rotating the directionnal vector 90°

    // InputTensor
    torch::Tensor inputTensor = torch::tensor({to_target_length, ddx, ddy, this->leg_up[0], this->leg_up[1], this->leg_up[2], this->leg_up[3], this->muscle_con[0], this->muscle_con[1]});
    // OutputTensor
    torch::Tensor output = brain->forward(inputTensor);

    // Update the info on the creature state
    for(int i=0;i<4;i++) {
        float temp = this->leg_up[i] = output[i].item<float>();
        this->leg_up[i] = temp > 0.0f ? 1.0f : 0.0f;
    }
    for(int i=0;i<2;i++) {
        float temp = output[i+4].item<float>();
        this->muscle_con[i] = temp > 0.0f ? 1.0f : 0.0f;
    }

    // Leg length management
    this->links[0].restLength = this->leg_len[(int) this->muscle_con[0]];
    if (this->leg_up[0]==0) this->vertices[1].fixed = true;
    else this->vertices[1].fixed = false;

    this->links[1].restLength = this->leg_len[(int) this->muscle_con[0]];
    if (this->leg_up[1]==0) this->vertices[2].fixed = true;
    else this->vertices[2].fixed = false;

    this->links[2].restLength = this->leg_len[(int) this->muscle_con[1]];
    if (this->leg_up[2]==0) this->vertices[3].fixed = true;
    else this->vertices[3].fixed = false;

    this->links[3].restLength = this->leg_len[(int) this->muscle_con[1]];
    if (this->leg_up[3]==0) this->vertices[4].fixed = true;
    else this->vertices[4].fixed = false;

    // Muscle length management
    this->muscles[0].restLength = this->muscle_len[(int) this->muscle_con[0]];
    this->muscles[1].restLength = this->muscle_len[(int) this->muscle_con[1]];
}

/*
 * Update the direction of the creature
 * @param restLength
 */
void Creature::update() {

    // Calculation of the midpoint between the front right leg (1) and rear right leg (3)
    sf::Vector2f mid = sf::Vector2f(
        (this->vertices[1].position.x + this->vertices[3].position.x) / 2.0f,
        (this->vertices[1].position.y + this->vertices[3].position.y) / 2.0f
    );

    // Calculation of the angle between the center of the body (0) and this midpoint
    float angl = std::atan2(
        mid.y - this->vertices[0].position.y,
        mid.x - this->vertices[0].position.x
    );

    this->dir = angl;
}

/*
 * Move the creature to the provided point
 * @param x
 * @param y
 */
void Creature::moveTo(float x, float y) {

    // Move the main body
    this->vertices[0].position = sf::Vector2f(x, y);
    this->vertices[1].position = sf::Vector2f(x + this->struc_len/2, y - this->struc_len/2);
    this->vertices[2].position = sf::Vector2f(x - this->struc_len/2, y - this->struc_len/2);
    this->vertices[3].position = sf::Vector2f(x + this->struc_len/2, y + this->struc_len/2);
    this->vertices[4].position = sf::Vector2f(x - this->struc_len/2, y + this->struc_len/2);

    // Reset speeds
    this->vertices[0].velocity = sf::Vector2f(0.0f, 0.0f);
    this->vertices[1].velocity = sf::Vector2f(0.0f, 0.0f);
    this->vertices[2].velocity = sf::Vector2f(0.0f, 0.0f);
    this->vertices[3].velocity = sf::Vector2f(0.0f, 0.0f);
    this->vertices[4].velocity = sf::Vector2f(0.0f, 0.0f);

    this->leg_up = {0, 0, 0, 0};
    this->muscle_con = {0, 0};
    this->dir = 0.0f;
}
