#include "board.h"
#include <iostream>

bool handle_events(sf::RenderWindow &screen);
void init_code();

int main(){

	sf::RenderWindow screen(sf::VideoMode(600, 400), "Codewords");
	screen.SetFramerateLimit(60);
	board game_board(20, 20);
	init_code();

	/*while(screen.IsOpened()){
		if(handle_events(screen))
			screen.Close();

		//print the board
		game_board.print_words(screen);

	}*/

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

void init_code(){
	std::vector <int> assignments;

	//init conversion to 0's
	for(int i=0; i<26; i++)
		assignments.push_back(i);

	for(int i=0; i<26; i++){
		int pos = (int)sf::Randomizer::Random(0, assignments.size()-1);
		int num = assignments[pos];
		conversion[num] = (char)('a' + i);
		assignments.erase(assignments.begin()+pos);
	}

	for(int i=0; i<26; i++)
		std::cout << conversion[i] << " ";
	std::cout << std::endl;

}
