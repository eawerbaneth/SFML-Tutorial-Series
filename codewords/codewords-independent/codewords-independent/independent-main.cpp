#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

char conversion[26];


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

void tile::draw(sf::RenderWindow* &screen){
	std::string bleh = "" + letter;
	sf::String str(bleh);
	str.SetScale(3, 3);
	str.SetPosition(coords.x+25, coords.y+25);
	screen->Draw(*sprite);
	screen->Draw(str);
}

tile::tile(sf::Vector2f n_coords, int n_num, sf::Image* &n_img){
	coords = n_coords;
	sprite->SetImage(*n_img);
	sprite->SetPosition(coords);
	num = n_num;
	for(int i=0; i<26; i++)
		if(num==conversion[i])
			real_let = i+90;
}

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
	sf::Vector2i start, end;
	std::string content;
	std::vector <tile> placement;
	std::string type;
};

void word::print_word(sf::RenderWindow* &screen){
	for(unsigned int i=0; i<placement.size(); i++)
		placement[i].draw(screen);
}

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

char word::get_tile(sf::Vector2f pos){
	for(unsigned int i=0; i<placement.size(); i++)
		if(placement[i].get_coords() == pos)
			return placement[i].get_real_let();
	return '0';
}

void word::erase_word(){
	placement.clear();
}

class board{
public:
	board(){}
	board(int n_rows, int n_cols);
	bool set_word(std::string n_word, sf::Vector2i start, sf::Vector2i end, sf::Image* &n_img);
	void print_words(sf::RenderWindow* &screen);

private:
	int rows, cols;
	std::vector <word*> words;
	std::vector<word> across[26];
	std::vector <word> down[26];
};

void board::print_words(sf::RenderWindow* &screen){
	for(unsigned int i=0; i<words.size(); i++){
		words[i]->print_word(screen);
	}
}

board::board(int n_rows, int n_cols){
	rows = n_rows;
	cols = n_cols;
}

bool board::set_word(std::string n_word, sf::Vector2i start, sf::Vector2i end, sf::Image* &n_img){
	//we're setting a word that spans across the board
	if(start.x == end.x){
		//check for same-row collisions
		for(unsigned int i=0; i<across[start.x].size(); i++){
			if(across[start.x][i].get_start().y < start.y-1 && across[start.x][i].get_end().y >= start.y-1)
				return false;
			if(across[start.x][i].get_start().y < end.y+1 && across[start.x][i].get_end().y >= end.y+1)
				return false;
		}
		//check to see that the intersections are alright
		int counter=0;
		for(unsigned int i=start.y; i<=end.y; i++){
			for(unsigned int k=0; k<down[i].size(); k++){
				char temp = down[i][k].get_tile(sf::Vector2f(start.x, i));
				if(temp!='0'&&temp!=n_word[counter])
					return false;
			}
			counter++;
		}
		//we're free to set this word
		word new_word(start, end, n_word, n_img);
		across[start.x].push_back(new_word);
		words.push_back(&across[start.x][across[start.x].size()-1]);
	}
	//we're setting a word that goes down
	else{
		//check for same-column collisions
		for(unsigned int i=0; i<down[start.y].size(); i++){
			if(down[start.y][i].get_start().x < start.y-1 && down[start.y][i].get_end().y >= start.y-1)
				return false;
			if(down[start.y][i].get_start().x < end.x+1 && down[start.y][i].get_end().x >= end.x+1)
				return false;
		}
		//check to see that the intersections are alright
		int counter=0;
		for(unsigned int i=start.x; i<=end.x; i++){
			for(unsigned int k=0; k<across[i].size(); k++){
				char temp = across[i][k].get_tile(sf::Vector2f(i, start.y));
				if(temp!='0'&&temp!=n_word[counter])
					return false;
			}
			counter++;
		}
		//we're free to set this word
		word new_word(start, end, n_word, n_img);
		down[start.y].push_back(new_word);
		words.push_back(&down[start.y][down[start.y].size()-1]);
	}
	return true;
}


bool handle_events(sf::RenderWindow &screen);

int main(){

	sf::RenderWindow screen(sf::VideoMode(600, 400), "Codewords");
	screen.SetFramerateLimit(60);

	while(screen.IsOpened()){
		if(handle_events(screen))
			screen.Close();
		

	}

	return 0;
}


bool handle_events(sf::RenderWindow &screen){
	sf::Event Event;
	while(screen.GetEvent(Event)){
		//handles exits
		if(Event.Type == sf::Event::Closed)
			return true;
		if(Event.Type==sf::Event::KeyPressed && Event.Key.Code == sf::Key::Escape)
			return true;
	}

	return false;

}
