#include "creature.h"

float score_distance(Creature* agent, sf::Vector2f goal) {
    return (std::sqrt(std::pow(agent->vertices[0].position.x - goal.x, 2) + std::pow(agent->vertices[0].position.y - goal.y, 2)));
}

Creature::Creature(float sx, float sy, int bodyColorSeed, std::string brainFile)
{   
    /// initialisation des paramètres de la créature


    // Masse des éléments de la créature
    this->body_mass = 10.0f;
    this->paw_mass = 2.0f;

    // initialisation des positions et vecteurs
    // longueur des membres de la creature
    this->muscle_strength = 100.0f;
    this->muscle_con = {0.0f, 0.0f};
    this->muscle_len = {50.0f, 37.5f};
    this->leg_len = {35.35f, 31.25f};
    this->struc_len = 50.0f;
    
    // corps, pattes 1-4
    // pattern des pattes
    // 2     1
    // | \ / |
    // |  0  |
    // | / \ |
    // 4     3          
    this->vertices = {
        Point(sx, sy, this->body_mass),          // 0 centre du corps
        Point(sx + this->struc_len/2, sy - this->struc_len/2, this->paw_mass), // 1 patte avant droite
        Point(sx - this->struc_len/2, sy - this->struc_len/2, this->paw_mass), // 2 patte avant gauche
        Point(sx + this->struc_len/2, sy + this->struc_len/2, this->paw_mass), // 3 patte arrière droite
        Point(sx - this->struc_len/2, sy + this->struc_len/2, this->paw_mass)  // 4 patte arrière gauche
    };

    // tableau des liens entre les points et leur longueur
    this->links = {
        Link(0, 1, this->leg_len[0]),
        Link(0, 2, this->leg_len[0]),
        Link(0, 3, this->leg_len[0]),
        Link(0, 4, this->leg_len[0]),
        Link(1, 3, this->struc_len),
        Link(2, 4, this->struc_len)
    };

    this->muscles = {
        Spring(1, 2, this->struc_len, this->muscle_strength),
        Spring(3, 4, this->struc_len, this->muscle_strength)
    };

    this->dir = 0.0f;
    this->leg_up = {0, 0, 0, 0};


    // Couleur de la creature
    this->colors = {
        sf::Color(0, 0, 255),
        sf::Color(0, 255, 255),
        sf::Color(255, 0, 255),
        sf::Color(34, 0, 25),
        unique_color_from_single_number(bodyColorSeed)
    };
    
    pawUp = sf::CircleShape(5);
    pawDown = sf::CircleShape(10);
    body = sf::CircleShape(20);
    eyeWhite = sf::CircleShape(8);
    eyeBlack = sf::CircleShape(2);
    
    pawUp.setOrigin({5, 5});
    pawDown.setOrigin({10, 10});
    body.setFillColor(this->colors[4]);
    body.setOrigin({20, 20});
    eyeWhite.setFillColor(sf::Color::White);
    eyeWhite.setOrigin({8, 8});
    eyeBlack.setFillColor(sf::Color::Black);
    eyeBlack.setOrigin({2, 2});
}

sf::Color Creature::unique_color_from_single_number(int number)
{
    return sf::Color((50*number+96)%255, (69*number+24)%255, (42*number+93)%255);
}

void Creature::brainUpdate(sf::Vector2f target, Brain * brain)
{
    float dx = target.x - this->vertices[0].position.x;
    float dy = target.y - this->vertices[0].position.y;

    float dist = std::sqrt(dx * dx + dy * dy);

    sf::Vector2f to_target = target - this->vertices[0].position;
    float to_target_length = std::sqrt(to_target.x * to_target.x + to_target.y * to_target.y);
    if (to_target_length != 0.0f) {
        to_target /= to_target_length;
    }
    sf::Vector2f dire = sf::Vector2f(std::cos(this->dir), std::sin(this->dir));

    float ddx = dire.x * to_target.x + dire.y * to_target.y;
    float ddy = -dire.y * to_target.x + dire.x * to_target.y;

    // InputTensor
    torch::Tensor inputTensor = torch::tensor({dist,ddx, ddy, this->leg_up[0], this->leg_up[1], this->leg_up[2], this->leg_up[3], this->muscle_con[0], this->muscle_con[1]});
    torch::Tensor output = brain->forward(inputTensor);

    for(int i=0;i<4;i++) {
        float temp = this->leg_up[i] = output[i].item<float>();
        //std::cout << temp << std::endl;
        this->leg_up[i] = temp > 0.0f ? 1.0f : 0.0f;
    }
    for(int i=0;i<2;i++) {
        float temp = output[i+4].item<float>();
        //std::cout << temp << std::endl;
        this->muscle_con[i] = temp > 0.0f ? 1.0f : 0.0f;
    }
}

void Creature::update(float dt)
{
    // gestion de la longueur des pattes
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

    // gestion de la longueur des muscles
    this->muscles[0].restLength = this->muscle_len[(int) this->muscle_con[0]];
    this->muscles[1].restLength = this->muscle_len[(int) this->muscle_con[1]];
    //std::cout << std::to_string(this->muscle_con[0]) << " " << std::to_string( this->muscle_con[1]) << std::endl;
    //std::cout << std::to_string((int) this->muscle_con[0]) << " " << std::to_string((int) this->muscle_con[1]) << std::endl;

    
        // Calcul du milieu entre la patte avant droite (1) et arrière droite (3)
    sf::Vector2f mid = sf::Vector2f(
        (this->vertices[1].position.x + this->vertices[3].position.x) / 2.0f,
        (this->vertices[1].position.y + this->vertices[3].position.y) / 2.0f
    );

    // Calcul de l'angle entre le centre du corps (0) et ce milieu
    float angl = std::atan2(
        mid.y - this->vertices[0].position.y,
        mid.x - this->vertices[0].position.x
    );

    this->dir = angl;
}

void Creature::moveTo(float x, float y) {

    // Déplacer le corps principal
    this->vertices[0].position = sf::Vector2f(x, y);
    this->vertices[1].position = sf::Vector2f(x + this->struc_len/2, y - this->struc_len/2);
    this->vertices[2].position = sf::Vector2f(x - this->struc_len/2, y - this->struc_len/2);
    this->vertices[3].position = sf::Vector2f(x + this->struc_len/2, y + this->struc_len/2);
    this->vertices[4].position = sf::Vector2f(x - this->struc_len/2, y + this->struc_len/2);

    // Réinitialiser les vitesses
    this->vertices[0].velocity = sf::Vector2f(0.0f, 0.0f);
    this->vertices[1].velocity = sf::Vector2f(0.0f, 0.0f);
    this->vertices[2].velocity = sf::Vector2f(0.0f, 0.0f);
    this->vertices[3].velocity = sf::Vector2f(0.0f, 0.0f);
    this->vertices[4].velocity = sf::Vector2f(0.0f, 0.0f);

    this->leg_up = {0, 0, 0, 0};
    this->muscle_con = {0, 0};
    this->dir = 0.0f;
}


void Creature::draw(sf::RenderWindow &window)
{
    // Dessin des pattes et des lignes corps-patte
    
    for (int i = 0; i < 4; ++i) {
        sf::CircleShape* paw = this->leg_up[i] ? &pawUp : &pawDown;
        paw->setFillColor(this->colors[i]);
        paw->setPosition(this->vertices[i+1].position);
        window.draw(*paw);

        // Ligne entre le corps et la patte
        std::array line =
        {
            sf::Vertex{this->vertices[0].position, this->colors[i]},
            sf::Vertex{this->vertices[i+1].position, this->colors[i]}
        };
        window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
    }

    // Dessin des muscles (lignes simples)
    for (int i = 0; i < 2; ++i) {
        std::array line =
        {
            sf::Vertex{this->vertices[i*2+1].position, this->colors[i]},
            sf::Vertex{this->vertices[i*2+2].position, this->colors[i]}
        };
        window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
    }

    // Dessin du corps
    
    body.setPosition(this->vertices[0].position);
    window.draw(body);

    // Dessin des yeux (réutilisation des formes)
    float eye_offset1 = this->dir - 0.3f;
    float eye_offset2 = this->dir + 0.3f;
    sf::Vector2f center = this->vertices[0].position;
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
