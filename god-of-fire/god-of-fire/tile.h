//tile.h
#include <SFML\Graphics.hpp>
#include <string>
#include <iostream>

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


private:
	float z;
	char type;
	sf::Sprite Sprite;
	bool corrupted;

};

class monk{
public:
	monk(){}
	monk(tile* tile, int Type, sf::Image *Image);

	//accessing privates
	int get_type(){return type;}
	sf::Vector2f get_pos(){return Sprite.GetPosition();}
	sf::Sprite get_sprite(){return Sprite;}


protected:
	int type;
	sf::Sprite Sprite;

};