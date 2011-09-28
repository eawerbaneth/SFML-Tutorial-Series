//tile.h

#ifndef _TILE_H_
#define _TILE_H

#include <SFML\Graphics.hpp>
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>

const int dim = 100;
const int citizen_dim = 75;

class fire{
public:
	fire::fire();
	fire::fire(sf::Vector2i the_source);

private:
	sf::Vector2i source;
	std::vector <sf::Vector2i> invuls;
};


class tile{
public:
	tile(){}
	tile(int row, int col, int Z, char Type, sf::Image *Image);
	//accessing privates
	char get_type(){return type;}
	sf::Vector2f get_pos(){return Sprite.GetPosition();}
	sf::Sprite get_sprite(){return Sprite;}
	sf::IntRect get_rect(){return Sprite.GetSubRect();}
	sf::Vector2i get_coords(){return coords;}
	bool is_occupied(){return occupied;}
	bool is_highlighted(){return highlighted;}
	bool is_corrupted(){return corrupted;}
	bool is_ignited(){return ignited;}
	bool occupy();
	bool ignite();
	void highlight();
	void corrupt();
	void update(std::vector <std::vector<tile*>> &map);
	

private:
	bool occupied;
	int  z;
	char type;
	sf::Sprite Sprite;
	bool corrupted;
	int corruption_cooldown;
	sf::Vector2i coords;
	bool ignited;
	int firedur;
	bool highlighted;
	int invulnerable;
};

struct path_helper{
	int dist;
	path_helper* prev;
	sf::Vector2i pos;
};


#endif
