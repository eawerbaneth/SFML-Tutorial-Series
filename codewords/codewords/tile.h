//generator.h
#include <SFML/Graphics.hpp>

class tile{
public:
	//constructors
	tile(){}
	tile(int row, int col, int new_size, sf::Image* base_img);
	//access privates
	int get_usage(){return usage;}
	bool is_start(){return start_of_word;}
	sf::Sprite* get_sprite(){return img;}
	int get_state(){return state;}
	char get_letter(){return letter;}
	int get_letter_usage(){return letter_usage;}
	int get_word_num(){return word_num;}
	void set_state(int new_state){state=new_state;}
	void set_letter(char new_letter);
	void set_word_num(int new_word_num){word_num = new_word_num;}
	void set_to_start(){start_of_word=true;}
	//utilities
	void remove_letter();
	void increment_usage(){usage++;}
	void reset_all();

private:
	int state; //0 if unfillable, 1 if empty, 2 if filled
	sf::Vector2i location;
	char letter;
	int usage, letter_usage;
	int word_num;
	bool start_of_word;
	int size;
	sf::Sprite* img;

};

tile::tile(int row, int col, int new_size, sf::Image* base_img){
	start_of_word = false;
	size = new_size;
	word_num = -1;
	usage = letter_usage = 0;
	location.x=col*size;
	location.y=row*size;
	img->SetImage(*base_img);
}

void tile::reset_all(){
	state = 1;
	letter_usage = 0;
	letter = NULL;
}

void tile::set_letter(char new_letter){
	if(state != 0) 
		letter = new_letter;
	state = 2;
	letter_usage = 2;
}

void tile::remove_letter(){
	if(letter!=NULL)
		letter = NULL;
	state = 1;
	letter_usage=0;
}