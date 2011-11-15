#include "board.h"

bool handle_events(sf::RenderWindow &screen);

int main(){

	sf::RenderWindow screen(sf::VideoMode(600, 400), "Codewords");
	screen.SetFramerateLimit(60);
	board game_board(20, 20);

	while(screen.IsOpened()){
		if(handle_events(screen))
			screen.Close();

		//print the board


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
