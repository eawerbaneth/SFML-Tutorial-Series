//player.h
#include <vector>
#include "misc.h"

class player{
public:
	//consturctors
	player();
	player(std::string new_name);

	//movement utilities
	void adjust(float x_offset, float y_offset, sf::Vector2f enemy_pos);

	//attack utilities
	bool punch(sf::Vector2f enemy_pos, std::string enemy_state);
	bool kick(sf::Vector2f enemy_pos, std::string enemy_state);
	void block();
	void release_state(sf::Vector2f enemy_pos);
	void prespec(sf::Vector2f enemy_pos);
	virtual special_attack* special(sf::Vector2f enemy_pos);

	//accessing privates
	std::string get_state();
	sf::Vector2f get_pos();
	bool take_damage(int damage, sf::Vector2f enemy_pos);

	//drawing utility
	sf::Sprite get_Sprite();
	sf::String get_name();
	sf::Sprite get_health();

protected:
	void destroy();
	sf::Sprite Sprite;
	std::string state;
	sf::String name;
	health_bar health;
	std::vector <sf::Image> states;
	float kick_offset;
	float punch_offset;
};

//SPECIALIZED CHARACTERS
class FIGHTBETH : public player{
public:
	FIGHTBETH();
	FIGHTBETH(std::string pos);
	blast *special(sf::Vector2f enemy_pos);

};

class FIGHTDAN : public player{
public:
	FIGHTDAN();
	FIGHTDAN(std::string pos);
	hawk *special(sf::Vector2f enemy_pos); 
	
};

