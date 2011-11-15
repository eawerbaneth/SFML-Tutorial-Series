#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

char conversion[26];

// -- tile class --
class tile{
public:
	tile(){}
	tile(sf::Vector2f n_coords, int n_num, sf::Image* &n_img);
	//sf::Sprite* get_sprite(){return sprite;}
	sf::Vector2f get_coords(){return coords;}
	void set_letter(char n_letter){letter=n_letter;}
	char get_real_let(){return real_let;}
	void draw(sf::RenderWindow* &screen);

private:
	sf::Vector2f coords;
	//sf::Image* img;
	sf::Sprite* sprite;
	int num;
	char letter;
	char real_let;
};

// -- draws new tile --
void tile::draw(sf::RenderWindow* &screen){
	std::string bleh = "" + letter;
	sf::String str(bleh);
	str.SetScale(3, 3);
	str.SetPosition(coords.x+25, coords.y+25);
	screen->Draw(*sprite);
	screen->Draw(str);
}

// -- tile constructor --
tile::tile(sf::Vector2f n_coords, int n_num, sf::Image* &n_img){
	coords = n_coords;
	sprite->SetImage(*n_img);
	sprite->SetPosition(coords);
	num = n_num;
	for(int i=0; i<26; i++)
		if(num==conversion[i])
			real_let = i+90;
}

// -- contains the words on the board --
class word{
public:
	word(){};
	word(sf::Vector2i n_start, sf::Vector2i n_end, std::string n_word, sf::Image* &n_img);
	char get_tile(sf::Vector2f pos);
	void erase_word();
	sf::Vector2i get_start(){return start;}
	sf::Vector2i get_end(){return end;}
	void print_word(sf::RenderWindow* &screen);

private:
	//start and end coordinates of the word
	sf::Vector2i start, end;
	//contains the actual word
	std::string content;
	//holes the tiles in the word
	std::vector <tile> placement;
	//across or down
	std::string type;
};

// -- draws all of the letters in the word --
void word::print_word(sf::RenderWindow* &screen){
	for(unsigned int i=0; i<placement.size(); i++)
		placement[i].draw(screen);
}

// -- word constructor (contains tile start position, tile end position
word::word(sf::Vector2i n_start, sf::Vector2i n_end, std::string n_word, sf::Image* &n_img){
	int i=0;
	content = n_word;
	start = n_start;
	end = n_end;

	for(int r = n_start.x; r<n_end.x; r++){
		for(int c = n_start.y; c < n_end.y; c++){
			tile temp(sf::Vector2f(r, c), content[i], n_img);
			placement.push_back(temp);
			i++;
		}
	}
	if(n_start.x == n_end.x)
		type = "across";
	else
		type = "down";
}

// -- returns the real letter of the given tile --
char word::get_tile(sf::Vector2f pos){
	for(unsigned int i=0; i<placement.size(); i++)
		if(placement[i].get_coords() == pos)
			return placement[i].get_real_let();
	return '0';
}

// -- clears the vector of tiles --
void word::erase_word(){
	placement.clear();
}
