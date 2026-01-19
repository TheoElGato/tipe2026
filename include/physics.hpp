/*
	physics.hpp

	Description:
		Header file for physics.cpp

	Author:
		R. Benichou
		A. Spadone
*/

#ifndef PHYSICS_H
#define PHYSICS_H

#include <SFML/System.hpp>
#include <cmath>
#include <map>
#include <vector>


class Creature; // Forward declaration


class Point {
public:
	Point(float x, float y, float mass);
	sf::Vector2f position;
	float mass;
	sf::Vector2f velocity;
	bool fixed;

	int id = -1;

	void setId(int id) {
		this->id = id;
	}
};

class Link {
public:
	Link(int a, int b, float restLength);
	int pointA;
	int pointB;
	float restLength;
};

class Spring {
public:
	Spring(int a, int b, float restLength, float springStrength);
	int pointA;
	int pointB;
	float restLength;
	float springStrength;

};

class PhysicsWorker {
public:
	PhysicsWorker();


	sf::Vector2f springForce(Point a, Point b, float springConstant, float restLength);
	void PBD(std::vector<Point>* objects, std::vector<Link> links, std::vector<Spring> springs, int numsubstep, float dt);

private:
	float distance(Point a, Point b);
};

#endif // PHYSICS_H
