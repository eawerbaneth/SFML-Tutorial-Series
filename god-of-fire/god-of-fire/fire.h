#ifndef _FIRE_H_
#define _FIRE_H

#include "tile.h"

class fire_node{
public:
	fire_node(){}
	fire_node(sf::Vector2i new_pos);
	bool update();

private:
	sf::Vector2i pos;
	int duration;

};

class fire_source{
public:
	fire_source(){}
	fire_source(sf::Vector2i the_source, int allowance, 
		std::vector <std::vector <tile*>> &map);
	bool update();

private:
	sf::Vector2i source;
	std::vector <sf::Vector2i> invuls;
	std::vector <sf::Vector2i> range;
	std::vector <fire_node*> nodes;
	void set_range(int allowance, std::vector <std::vector <tile*>> &map);
};

class fire{
public:
	fire(){}
	void update();

private:
	std::vector <fire_source*> fires;
};

#endif