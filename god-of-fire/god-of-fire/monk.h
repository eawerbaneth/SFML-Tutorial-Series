#include "tile.h"

class corrupted;

class monk{
public:
	monk(){}
	monk(tile* my_tile, sf::Image *Image, tile* destination);
	monk(sf::Vector2i my_tile, sf::Vector2i my_dest, sf::Vector2f my_pos, sf::Image *Image);
	monk(corrupted* c, sf::Image *Image);


	//accessing privates
	sf::Vector2f get_pos(){return Sprite.GetPosition();}
	sf::Sprite get_sprite(){return Sprite;}
	sf::IntRect get_rect(){return Sprite.GetSubRect();}
	sf::Vector2i get_tile(){return tilecoords;}

	//neutral utilities
	bool request_occupy(tile* new_tile);
	//sf::Vector2i update();
	void set_dest(sf::Vector2i newcoords){destcoords = newcoords;}
	sf::Vector2i get_dest(){return destcoords;}
	bool dest_reached(){return tilecoords==destcoords;}
	void set_path(std::vector <sf::Vector2i> new_path){path = new_path;}
	std::vector <sf::Vector2i> get_path(){return path;}
	void ignite();
	bool update(sf::Vector2i &next);

protected:
	sf::Image *my_Image;
	sf::Sprite Sprite;
	sf::Vector2i tilecoords;
	sf::Vector2i destcoords;
	std::vector <sf::Vector2i> path;
	bool ignited;
	int death_walk;

};

class faithful: public monk{
public:
	faithful(){}
	faithful(tile* my_tile, sf::Image *Image/*, tile* destination*/);
	faithful(monk* old_monk, sf::Image* Image);
	//accessing privates
	bool is_selected(){return selected;}
	//detonation utilites
	std::vector <sf::Vector2i> get_range(std::vector <std::vector <tile*>> &map);
	bool detonate(tile* target);
	bool select();
	//general update
	bool update(std::vector <std::vector <tile*>> &map, fire &the_fire);

private:
	bool selected;
	bool detonated;
	std::vector <sf::Vector2i> deto_range;

};

class corrupted: public monk{
public:
	corrupted(){}
	corrupted(tile* my_tile, sf::Image *Image, tile* destination);
	corrupted(monk* old_monk, sf::Image* Image);
	void c_update(std::vector <std::vector <tile*>> &map);

};