#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

//disable warnings
#pragma warning(disable:4244)
#pragma warning(disable:4018)
#pragma warning(disable:4996)


bool DEBUGGING = true;
/*Board is 15 by 10
Margins of 100 px on each side
800 - 200 = 600 / 15 = 40 for each tile

*/

//our conversion codex
vector<char> conversions(26, 'a');

//prototypes
bool HandleEvents(sf::RenderWindow* &window);
class Word;

class WordsHash{
private:
	std::vector <std:: vector <std::string>> table;

public:
	WordsHash(){}
	WordsHash(string filename){
		std::ifstream input(filename.c_str());
		std::string reader;

		for(unsigned int i=0; i<20; i++){
			std::vector <std::string> temp;
			table.push_back(temp);
		}
	
		while(!input.eof()){
			input >> reader;
			if(reader.length() > 3 && reader.length() < 20)
				table[reader.length()].push_back(reader);
		}
	}
	std::string get_random_word(int i){
		int rand = sf::Randomizer::Random(0, table[i].size()-1);
		string word = table[i][rand];
		table[i].erase(table[i].begin() + rand);
		return word;
	}
	int get_words(int i){return table[i].size();}
};

class Tile{
public:
	Tile(){letter = '0'; /*set letter to unoccupied*/}
	Tile(sf::Vector2i i){
		letter = '0'; 
		index = i;
		position = sf::Vector2f(100 + index.x*40, 100 + index.y*40);
		rect = sf::FloatRect((float)position.x, (float)position.y, (float)position.x + 40, (float)position.y + 40);
		img = sf::Shape::Rectangle(position, sf::Vector2f(position.x + 40, position.y + 40), sf::Color::White);
		selected = false;
		guess = '0';
		code = "";
		//img.SetColor(sf::Color::Yellow);
		can_change = true;
		bottom = false;
	}
	//bool IsOccupied(){return letter != '0'; /*return true if letter is occupied*/}
	bool Available(char candidate){
		if(letter == candidate || letter == '0')
			return true;
		else return false;
	}
	void SetLetter(char l){letter = l;}
	//void SetMember(Word* word){member_of.push_back(word);}
	bool WasClicked(sf::Vector2f mouse_coords){
		if(rect.Contains(mouse_coords.x, mouse_coords.y) && letter != '0'){
			img.SetColor(sf::Color::Yellow);
			return true;
		}
		
		return false;

	}
	vector <Word*> GetWords(){return member_of;}
	void DrawTile(sf::RenderWindow* &window){
		//draw background
		window->Draw(img);

		//draw code in top right corner
		sf::String c;
		c.SetFont(sf::Font::GetDefaultFont());
		c.SetText(code);
		c.SetSize(13);
		c.SetColor(sf::Color::Black);
		c.SetPosition(position.x + 15, position.y);
		window->Draw(c);

		if(DEBUGGING && letter != '0'){
			sf::String d;
			d.SetText(display_letter);
			d.SetColor(sf::Color::Red);
			d.SetPosition(position.x + 5, position.y + 5);
			window->Draw(d);
		}

		//if there is a guess, draw it
		if(guess != '0'){
			sf::String g;
			stringstream out;
			out << guess;

			g.SetText(out.str());
			if(can_change)
				g.SetColor(sf::Color::Blue);
			else
				g.SetColor(sf::Color::Black);
			g.SetPosition(position.x + 5, position.y + 5);
			window->Draw(g);
		}

	}
	void Guess(char g){
		if(can_change)
			guess = g;
	}
	void ClearGuess(){
		if(can_change)
			guess = '0';
	}
	sf::Vector2i GetIndex(){return index;}
	void ClearSelection(){
		if(letter == '0')
			return;

		if(bottom)
			img.SetColor(sf::Color::Cyan);
		else
			img.SetColor(sf::Color::White);
	}
	void SetCode(){
		if(letter == '0'){
			img.SetColor(sf::Color::Black);
			return;
		}

		for(unsigned int i = 0; i < conversions.size(); i++){
			if(conversions[i] == letter){
				stringstream out;
				out << i;
				code = out.str();
			}
		}

		//set real letter to display letter
		stringstream out;
		out << letter;
		display_letter = out.str();
	}
	void SetBottom(){img.SetColor(sf::Color::Cyan); can_change = true; bottom = true;}
	char GetLetter(){return letter;}
	char SetOriginal(){
		can_change = false;
		guess = letter;
		return letter;
	}

private:
	bool selected;
	char letter;
	string code;
	string display_letter;
	char guess;
	vector <Word*> member_of;	
	sf::Vector2i index;
	sf::Vector2f position;
	sf::FloatRect rect;
	sf::Shape img;
	bool can_change;
	bool bottom;
};

class Word{/* this is more of an auxiliary class to bridge the gap between board and tiles*/
public:
	Word(){}
	Word(string w, bool h, vector<Tile*> t){
		horizontal = h;
		word = w;
		tiles = t;
		index = tiles[0]->GetIndex();
	}
	string GetWord(){return word;}
	sf::Vector2i GetIndex(){return index;}
	bool GetOrientation(){return horizontal;}

private:
	bool horizontal; //true == horizontal, false == vertical
	string word;
	sf::Vector2i index;
	vector <Tile*> tiles;

};

class Board{//handle all of the tiles on the board, keep track of words that have been filled in, etc.
public:
	Board(){
		//initialize the 15x10 space of tiles (we are doing row, col format)
		for(unsigned int r = 0; r < 10; r++){
			vector <Tile> row;
			for(unsigned int c = 0; c < 15; c++)
				row.push_back(Tile(sf::Vector2i(c, r)));
			tileset.push_back(row);
		}

		//fill the board with as many words as we can generate
		FillBoard();

		//figure out our secret code
		GenerateCode();

		//do the tiles at the bottom of the board
		SetBottomTiles();
		FillGiven();

		selected_tile = NULL;

	};

	void FillGiven(){
		for(int i = 0; i < bottom_tiles.size(); i++){
			for(int k = 0; k < given.size(); k++){
				if(bottom_tiles[i].GetLetter() == given[k])
					bottom_tiles[i].SetOriginal();
			}
		}
	}

	vector<Tile*> CreateWord(string word, sf::Vector2i first_pos, bool horizontal){
		vector<Tile*> tiles;

		for(int i = 0; i < word.length(); i++){
			//remember tileset is ordered row, col
			tileset[first_pos.y][first_pos.x].SetLetter(word[i]);
			tiles.push_back(&tileset[first_pos.y][first_pos.x]);

			if(horizontal)
				first_pos.x++;
			else
				first_pos.y++;
			
		}

		return tiles;

	}

	bool CheckAvailability(string word, sf::Vector2i first_pos, bool horizontal){
		//make sure that we're not going to go out of bounds from our position
		if(first_pos.x < 0 || first_pos.y < 0) //first_pos.x + word.length() >= tileset[0].size() || first_pos.y + word.length() >= tileset.size())
			return false;
		//horizontal words -> y remains constant, vertical words -> x remains constant
		if(horizontal){
			if(first_pos.x + word.length() >= tileset[0].size()){
				return false;
			}
		}
		else{
			if(first_pos.y + word.length() >= tileset.size()){
				return false;
			}

		}

		
		for(int i = 0; i < word.length(); i++){
			if(!tileset[first_pos.y][first_pos.x].Available(word[i]))
				return false;

			//if we're on the first letter, we need to check to make sure there isn't a letter right before us that is going to mess us up
			if(i == 0){
				if(horizontal){
					//for now, we want it to be completely clear [can check for a letter that still makes it a word later]
					if(first_pos.x - 1 >= 0)
						if(!tileset[first_pos.y][first_pos.x - 1].Available('0'))
							return false;
				}
				else{
					if(first_pos.y > 0)
						if(!tileset[first_pos.y - 1][first_pos.x].Available('0'))
							return false;
				}

			}

			//same thing with last letter
			if(i == word.length() - 1){
				if(horizontal){
					if(first_pos.x < tileset[0].size() - 1 && !tileset[first_pos.y][first_pos.x + 1].Available('0'))
						return false;
				}
				else{
					if(first_pos.y < tileset.size() - 1 && !tileset[first_pos.y + 1][first_pos.x].Available('0'))
						return false;
				}

			}

			//all letters, check below and above(be that column or row)
			if(tileset[first_pos.y][first_pos.x].GetLetter() == '0'){
				if(horizontal){//check row above and row below
					if(first_pos.y > 0)
						if(!tileset[first_pos.y - 1][first_pos.x].Available('0'))
							return false;
					if(first_pos.y < tileset.size() - 1)
						if(!tileset[first_pos.y + 1][first_pos.x].Available('0'))
							return false;
				}
				else{
					if(first_pos.x > 0)
						if(!tileset[first_pos.y][first_pos.x - 1].Available('0'))
							return false;
					if(first_pos.x < tileset[0].size() - 1)
						if(!tileset[first_pos.y][first_pos.x + 1].Available('0'))
							return false;
				}

			}

			if(horizontal)
				first_pos.x++;
			else
				first_pos.y++;
		}

		return true;
	}

	bool FindLocation(string candidate, bool horizontal){
		for(int i = 0; i < words.size(); i++){
			//only consider words with the opposite orientation
			if(horizontal == words[i].GetOrientation())
				continue;
			//search for a common letter in the word
			string word = words[i].GetWord();
			for(int c = 0; c < word.length(); c++){
				for(int k = 0; k < candidate.length(); k++){
					if(word[c] == candidate[k]){
						//they share a common letter, figure out prospective index and check availability
						sf::Vector2i index = words[i].GetIndex();
						//we know this word is opposite orientation of horizontal
						if(horizontal){//this word is vertical, we're going to back y up c spaces (x remains constant)
							index.y += c;
							index.x -= k;

						}
						else{//this word is horizontal (y remains constant), back up x c spaces
							index.x += c;
							index.y -= k;

						}
						//check for availability
						if(CheckAvailability(candidate, index, horizontal)){
							vector<Tile*> tset = CreateWord(candidate, index, horizontal);
							words.push_back(Word(candidate, horizontal, tset));
							return true;
						}
					}
				}
			}
		}

		return false;
	}

	void FillBoard(){
		//read in dictionary of words
		WordsHash dictionary("words.txt");

		//start out by picking a random 8-letter word horizontally across row 5 (coords are going to be 4, 5)
		string word = dictionary.get_random_word(8);
		//use create word first, then add word to our vector of words
		vector<Tile*> tset = CreateWord(word, sf::Vector2i(4, 5), true);
		words.push_back(Word(word, true, tset));

		int spec_offset = 0;
		//we're just going to give them the first three letters of this word as the starting letters
		for(int x = 0; x < 3; x++){
			char prospective_given = tileset[5][4 + x + spec_offset].SetOriginal();
			bool original = true;
			for(int z = 0; z < given.size(); z++){
				if(prospective_given == given[z]){
					spec_offset++;
					original = false;
					x--;
				}
			}
			if(original){
				given.push_back(prospective_given);
			}

		}


		//now continue filling in until we get ten failures
		int failures = 0;
		bool orientation = false;

		for(int i = 12; i > 3; i--){
			failures = 0;
			while(failures < 30){
				//pick a random word to be oriented opposite of whatever the last word was between 5 and 12 letters
				word = dictionary.get_random_word(i);
				//go through existing words and look for a place it can slot in
				if(FindLocation(word, orientation)){
					orientation = !orientation;
					failures = 0;
				}
				else
					failures++;
			}
		}

	}

	void Click(sf::Vector2f mouse_coords){
		//clear any current clicks
		if(selected_tile != NULL)
			selected_tile->ClearSelection();
		selected_tile = NULL;

		for(int r = 0; r < tileset.size(); r++){
			for(int c = 0; c < tileset[r].size(); c++)
				if(tileset[r][c].WasClicked(mouse_coords))
					selected_tile = &tileset[r][c];
		}

		for(int i = 0; i < bottom_tiles.size(); i++)
			if(bottom_tiles[i].WasClicked(mouse_coords)){
				selected_tile = &bottom_tiles[i];
			}
	}

	//eventually you just need to pick 3 random contiguous letters
	void SetBottomTiles(){
		//board is 15x10, we are in 800x600, free space is going to be 100 + 600 + 100, y index is 700/80 18, x is 1
		for(int i = 0; i < conversions.size(); i++){
			if(i < 13)
				bottom_tiles.push_back(Tile(sf::Vector2i(i + 1, 12)));
			else
				bottom_tiles.push_back(Tile(sf::Vector2i(i - 13 + 1, 13)));
			bottom_tiles[i].SetLetter(conversions[i]);
			bottom_tiles[i].SetBottom();
			bottom_tiles[i].SetCode();
		}

	}

	void DrawTiles(sf::RenderWindow* &window){
		for(unsigned int i = 0; i < bottom_tiles.size(); i++)
			bottom_tiles[i].DrawTile(window);

		for(unsigned int r = 0; r < tileset.size(); r++){
			for(unsigned int c = 0; c < tileset[r].size(); c++)
				tileset[r][c].DrawTile(window);
		}
	}

	void GenerateCode(){
		//fill up a vector with numbers 0 - 25
		vector <int> helper;
		for(int i = 0; i < 26; i++)
			helper.push_back(i);

		//go through alphabet (a-z) and assign a random number to each letter
		for(int l = 0; l < 26; l++){
			int random = sf::Randomizer::Random(0, helper.size() - 1);
			//assign letter to random position
			conversions[helper[random]] = (char)(l + 97);
			//erase that position
			helper.erase(helper.begin() + random);
		}

		//go through finalized tiles and set their codes
		for(int r = 0; r < tileset.size(); r++){
			for(int c = 0; c < tileset[r].size(); c++)
				tileset[r][c].SetCode();
		}

	}

	void Guess(char guess){
		if(selected_tile == NULL)
			return;

		//screen for valid key-presses (a-z and backspace)
		if(!(((int)guess >= 97 && (int)guess <= 122) || (int)guess == 8))
			return;


		selected_tile->Guess(guess);

	}

private:
	vector <Word> words;
	vector<vector <Tile>> tileset;
	vector <Tile> bottom_tiles;
	Tile* selected_tile;
	vector <char> given;

};

/*
CLASSES:
tile - represents a position on the board - contains a letter, position, and pointer(s) to words it is a member of
word - words on the board, has a type (vertical/horizontal), a string (word itself), and a start pos (vector 2)
board - contains all tiles

STEPS
1. Build crossword
   -- pick random words and fill up puzzle until x-failures
2. Generate code-mappings
3. Interface
*/



//main loop
void GameLoop(){
	sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(800, 800), "Codewords");

	//display loading screen
	sf::String Loading("Loading...");
	Loading.SetPosition(200, 200);
	Loading.SetSize(48);
	window->Draw(Loading);
	window->Display();


	Board board;
	const sf::Input& input = window->GetInput();
	
	while(window->IsOpened()){
		sf::Event ev;
		while(window->GetEvent(ev)){
			//handle exits
			if(ev.Type == sf::Event::Closed)
				window->Close();
			if(ev.Type == sf::Event::KeyPressed && ev.Key.Code == sf::Key::Escape)
				window->Close();
			//get keyboard events
			else if(ev.Type == sf::Event::KeyPressed){
				board.Guess((char)ev.Key.Code);
				
			}
		}

		if(input.IsMouseButtonDown(sf::Mouse::Left))
			board.Click(sf::Vector2f(input.GetMouseX(), input.GetMouseY()));
		
		window->Clear();
		board.DrawTiles(window);

		window->Display();
	}


}


int main(){

	GameLoop();

	return 0;

}

bool HandleEvents(sf::RenderWindow* &window){
	sf::Event ev;
	while(window->GetEvent(ev)){
		//handle exits
		if(ev.Type == sf::Event::Closed)
			return false;
		if(ev.Type == sf::Event::KeyPressed && ev.Key.Code == sf::Key::Escape)
			return false;
	}

	return true;

}