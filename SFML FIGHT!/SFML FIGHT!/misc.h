//health_bar.h
#include <SFML/Graphics.hpp>
#include <string>

class health_bar{
public:
	health_bar();
	bool take_damage(int damage);
	sf::Sprite get_bar();
	void set_pos(float x);

private:
	sf::Sprite bar;
	int health;

};

//Special Attacks
class special_attack{
public:
	special_attack();
	virtual sf::Sprite get_image(float ElapsedTime, bool &alive)=0;
	virtual bool collides(sf::Vector2f enemy_pos, sf::Vector2f enemy_size, 
		std::string enemy_state)=0;
};

//Beth's Special
class blast: public special_attack{
public:
	blast();
	blast(sf::Vector2f pos, bool dir);
	~blast();
	sf::Sprite get_image(float ElapsedTime, bool &alive);
	bool collides(sf::Vector2f enemy_pos, sf::Vector2f enemy_size, std::string enemy_state);

private:
	sf::Sprite image;
	bool direction;
	bool advance(float ElapsedTime);
};

class hawk: public special_attack{
public:
	hawk();
	hawk(sf::Vector2f my_pos, sf::Vector2f enemy_pos);
	~hawk();
	sf::Sprite get_image(float ElapsedTime, bool &alive);
	bool collides(sf::Vector2f enemy_pos, sf::Vector2f enemy_size, std::string enemy_state);

private:
	sf::Sprite image;
	std::vector <sf::Vector2f> path;
	void generate_parabola(sf::Vector2f enemy_pos, bool dir);
	bool advance(float ElapsedTime);
};

