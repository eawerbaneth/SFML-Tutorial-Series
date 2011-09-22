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
	bool occupy();
	bool ignite();
	void highlight();
	

private:
	bool occupied;
	int  z;
	char type;
	sf::Sprite Sprite;
	bool corrupted;
	sf::Vector2i coords;
	bool ignited;
	int firedur;
	bool highlighted;
};

class monk{
public:
	monk(){}
	monk(tile* my_tile, sf::Image *Image, tile* destination);
	monk(sf::Vector2i my_tile, sf::Vector2i my_dest, sf::Vector2f my_pos, sf::Image *Image);

	//accessing privates
	sf::Vector2f get_pos(){return Sprite.GetPosition();}
	sf::Sprite get_sprite(){return Sprite;}
	sf::IntRect get_rect(){return Sprite.GetSubRect();}
	sf::Vector2i get_tile(){return tilecoords;}

	//neutral utilities
	bool request_occupy(tile* new_tile);
	sf::Vector2i update();
	void set_dest(sf::Vector2i newcoords){destcoords = newcoords;}
	sf::Vector2i get_dest(){return destcoords;}
	bool dest_reached(){return tilecoords==destcoords;}
	void set_path(std::vector <sf::Vector2i> new_path){path = new_path;}

protected:
	sf::Image *my_Image;
	sf::Sprite Sprite;
	sf::Vector2i tilecoords;
	sf::Vector2i destcoords;
	std::vector <sf::Vector2i> path;

};

class faithful: public monk{
public:
	faithful(){}
	faithful(tile* my_tile, sf::Image *Image, tile* destination);
	faithful(monk* old_monk, sf::Image* Image);
	//accessing privates
	bool is_selected(){return selected;}
	//detonation utilites
	std::vector <sf::Vector2i> get_range(std::vector <std::vector <tile*>> &map);
	bool detonate(tile* target);
	bool select();
	//general update
	bool update(std::vector <std::vector <tile*>> &map);

private:
	bool selected;
	bool detonated;
	int death_walk;
	std::vector <sf::Vector2i> deto_range;

};

class corrupted: public monk{
public:
	corrupted(){}
	corrupted(tile* my_tile, sf::Image *Image, tile* destination);

};


struct path_helper{
	int dist;
	path_helper* prev;
	sf::Vector2i pos;
};


#endif
