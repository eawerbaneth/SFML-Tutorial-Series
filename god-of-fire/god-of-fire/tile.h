//tile.h
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
	bool occupy();


private:
	bool occupied;
	int  z;
	char type;
	sf::Sprite Sprite;
	bool corrupted;
	sf::Vector2i coords;

};

class monk{
public:
	monk(){}
	monk(tile* my_tile, int Type, sf::Image *Image, tile* destination);

	//accessing privates
	int get_type(){return type;}
	sf::Vector2f get_pos(){return Sprite.GetPosition();}
	sf::Sprite get_sprite(){return Sprite;}
	sf::IntRect get_rect(){return Sprite.GetSubRect();}
	sf::Vector2i get_tile(){return tilecoords;}

	bool request_occupy(tile* new_tile);
	sf::Vector2i update();
	void set_dest(sf::Vector2i newcoords){destcoords = newcoords;}
	sf::Vector2i get_dest(){return destcoords;}
	bool dest_reached(){return tilecoords==destcoords;}
	void set_path(std::vector <sf::Vector2i> new_path){path = new_path;}

protected:
	int type;
	sf::Sprite Sprite;
	sf::Vector2i tilecoords;
	sf::Vector2i destcoords;
	std::vector <sf::Vector2i> path;

};

class faithful: public monk{
public:


};

class corrupted: public monk{
public:


};

