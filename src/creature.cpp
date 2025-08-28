#include "creature.h"

Creature::Creature(float sx, float sy, int bodyColorSeed, std::string brainFile)
{   
    /// initialisation des paramètres de la créature


    // Masse des éléments de la créature
    this->body_mass = 10.0f;
    this->paw_mass = 2.0f;

    // initialisation des positions et vecteurs
    // longueur des membres de la creature
    this->muscle_strength = 100.0f;
    this->muscle_con = std::make_tuple(0.0f, 0.0f);
    this->muscle_len = std::make_tuple(50.0f, 37.5f);
    this->leg_len = std::make_tuple(35.35f, 31.25f);
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
        Link(this->vertices[0], this->vertices[1], std::get<0>(this->muscle_len)),
        Link(this->vertices[0], this->vertices[2], std::get<0>(this->muscle_len)),
        Link(this->vertices[0], this->vertices[3], std::get<0>(this->muscle_len)),
        Link(this->vertices[0], this->vertices[4], std::get<0>(this->muscle_len)),
        Link(this->vertices[1], this->vertices[3], this->struc_len),
        Link(this->vertices[2], this->vertices[4], this->struc_len)
    };

    this->muscles = {
        Spring(this->vertices[1], this->vertices[2], this->struc_len, this->muscle_strength),
        Spring(this->vertices[3], this->vertices[4], this->struc_len, this->muscle_strength)
    };

    this->dir = 0.0f;
    this->leg_up = {false, false, false, false};


    // Couleur de la creature
    this->colors = {
        sf::Color(0, 0, 255),
        sf::Color(0, 255, 255),
        sf::Color(255, 0, 255),
        sf::Color(34, 0, 25),
        unique_color_from_single_number(bodyColorSeed)
    };
}

sf::Color Creature::unique_color_from_single_number(int number)
{
    return sf::Color((50*number+96)%255, (69*number+24)%255, (42*number+93)%255);
}

void Creature::update(float dt)
{
    // gestion de la longueur des pattes
    this->links[0].restLength = std::get<0>(this->leg_len);
    this->vertices[1].fixed = !this->leg_up[0];

    this->links[1].restLength = std::get<0>(this->leg_len);
    this->vertices[2].fixed = !this->leg_up[1];

    this->links[2].restLength = std::get<1>(this->leg_len);
    this->vertices[3].fixed = !this->leg_up[2];

    this->links[3].restLength = std::get<1>(this->leg_len);
    this->vertices[4].fixed = !this->leg_up[3];

    // gestion de la longueur des muscles
    this->muscles[0].restLength = std::get<0>(this->muscle_len);
    this->muscles[1].restLength = std::get<1>(this->muscle_len);

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

void Creature::draw(sf::RenderWindow &window)
{
    // Préparer les formes à l'avance pour éviter les allocations répétées
    sf::CircleShape pawUp(5);
    pawUp.setOrigin({5, 5});
    sf::CircleShape pawDown(10);
    pawDown.setOrigin({10, 10});

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
    static sf::CircleShape body(20);
    body.setFillColor(this->colors[4]);
    body.setOrigin({20, 20});
    body.setPosition(this->vertices[0].position);
    window.draw(body);

    // Dessin des yeux (réutilisation des formes)
    float eye_offset1 = this->dir - 0.3f;
    float eye_offset2 = this->dir + 0.3f;
    sf::Vector2f center = this->vertices[0].position;
    sf::Vector2f eye1 = center + sf::Vector2f(20 * std::cos(eye_offset1), 20 * std::sin(eye_offset1));
    sf::Vector2f eye2 = center + sf::Vector2f(20 * std::cos(eye_offset2), 20 * std::sin(eye_offset2));

    static sf::CircleShape eyeWhite(8);
    eyeWhite.setFillColor(sf::Color::White);
    eyeWhite.setOrigin({8, 8});
    eyeWhite.setPosition(eye1);
    window.draw(eyeWhite);
    eyeWhite.setPosition(eye2);
    window.draw(eyeWhite);

    static sf::CircleShape eyeBlack(2);
    eyeBlack.setFillColor(sf::Color::Black);
    eyeBlack.setOrigin({2, 2});
    eyeBlack.setPosition(eye1);
    window.draw(eyeBlack);
    eyeBlack.setPosition(eye2);
    window.draw(eyeBlack);
}
