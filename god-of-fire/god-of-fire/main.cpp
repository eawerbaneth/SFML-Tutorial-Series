//main.cpp

#include <vector>
#include <fstream>
#include "tile.h"

sf::Image tileset;

//return true if exit command received
bool handle_events(sf::RenderWindow &screen, sf::View &View);
void keyboard_input(sf::RenderWindow &screen, sf::View &View);
void police_boundaries(sf::View &View, sf::Vector2i &mapsize);
bool readmap(std::vector <std::vector<tile*>> &map);
void load_monks(std::vector <monk*> &monks, std::vector <std::vector<tile*>> &map);
void printmap(sf::RenderWindow &screen, std::vector <std::vector <tile*>> &map);
void printmapdynamic(sf::RenderWindow &screen, std::vector <std::vector <tile*>> &map, sf::View &View);
void printmonks(sf::RenderWindow &screen, std::vector <monk*> &monks);

int main(){
	//initalize the game window
	sf::RenderWindow screen(sf::VideoMode(800, 600), "God of Fire");
	screen.SetFramerateLimit(60);

	//fun initializations
	sf::Vector2f old_mouse_coords;
	sf::Vector2f mouse_coords;
	std::vector <std::vector <tile*>> map;
	std::vector <monk*> monks;
	//if we failed to load our map, exit program
	if(!readmap(map))
		return 0;
	load_monks(monks, map);
	
	sf::Vector2i mapsize(map[0].size()*dim, map.size()*dim/4+dim);
	
	sf::View View=screen.GetDefaultView();
	while(screen.IsOpened()){

		if(handle_events(screen, View))
			screen.Close();

		//mouse control
		old_mouse_coords = screen.ConvertCoords((unsigned)mouse_coords.x, (unsigned)mouse_coords.y);
		mouse_coords = screen.ConvertCoords(screen.GetInput().GetMouseX(), 
			screen.GetInput().GetMouseY());
		if(screen.GetInput().IsMouseButtonDown(sf::Mouse::Left))
			View.Move(-1.5*(mouse_coords.x-old_mouse_coords.x), 
			-1.5*(mouse_coords.y-old_mouse_coords.y));

		//keyboard control
		keyboard_input(screen, View);

		//maintain boundaries
		police_boundaries(View, mapsize);

		//display
		screen.Clear();
		screen.SetView(View);	
		//printmap(screen, map);
		printmapdynamic(screen, map, View);
		printmonks(screen, monks);
		screen.SetView(screen.GetDefaultView());
		screen.Display();

	}

	return 0;
}

//print map dynamically
void printmapdynamic(sf::RenderWindow &screen, std::vector <std::vector <tile*>> &map, 
	sf::View &View){
	unsigned int row = (View.GetRect().Top)/dim;
	if(row<0) row=0;

	for(row; (row<map.size()&&row<(View.GetRect().Bottom+dim)/(dim/4)); row++){
		unsigned int col = (View.GetRect().Left)/dim;
		if(col<0) col=0;
		for(col; (col<map[row].size()&&col<View.GetRect().Right); col++)
			screen.Draw(map[row][col]->get_sprite());
	}

}

//print entire map
void printmap(sf::RenderWindow &screen, std::vector <std::vector <tile*>> &map){
	for(unsigned int i=0; i<map.size(); i++)
		for(unsigned int k=0; k<map[i].size(); k++){
			screen.Draw(map[i][k]->get_sprite());
			//screen.Display();
		}
}



void printmonks(sf::RenderWindow &screen, std::vector <monk*> &monks){
	for(unsigned int i=0; i<monks.size(); i++)
		screen.Draw(monks[i]->get_sprite());
}

void load_monks(std::vector <monk*> &monks, std::vector <std::vector<tile*>> &map){
	int num_monks = map.size()*map[0].size()/20;
	sf::Image monk_sprites;
	monk_sprites.LoadFromFile("imgs/monks.png");

	for(int i=0; i<num_monks; i++){
		int randx = sf::Randomizer().Random(0, map.size()-1);
		int randy = sf::Randomizer().Random(0, map[0].size()-1);
		monks.push_back(new monk(map[randx][randy], 0, &monk_sprites));
	}

}

//read our file
bool readmap(std::vector <std::vector <tile*>> &map){
	//boring initializations
	std::ifstream input;
	
	if(!tileset.LoadFromFile("imgs/modtiles2.png"))
		return false;

	input.open("maps/proxymap.txt");
	if(!input.is_open())
		return false;

	while(!input.eof()){
		std::vector <tile*> row;
		char line[256];
		input.getline(line, 256);

		for(int i=0; i<input.gcount()-1; i++)
			row.push_back(new tile(map.size(), i, 0, line[i], &tileset));
		map.push_back(row);
	}

	return true;
}

void police_boundaries(sf::View &View, sf::Vector2i &mapsize){
		sf::Vector2f center = View.GetCenter();
		sf::Vector2f half_size = View.GetHalfSize();
		if(center.x-half_size.x<0)
			View.SetCenter(half_size.x, center.y);
		if(center.x+half_size.x>mapsize.x)
			View.SetCenter(mapsize.x-half_size.x, center.y);
		center = View.GetCenter();
		if(center.y-half_size.y<0)
			View.SetCenter(center.x, half_size.y);
		if(center.y+half_size.y>mapsize.y)
			View.SetCenter(center.x, mapsize.y-half_size.y);

}

void keyboard_input(sf::RenderWindow &screen, sf::View &View){
	if(screen.GetInput().IsKeyDown(sf::Key::Right))
		View.Move(20, 0);
	if(screen.GetInput().IsKeyDown(sf::Key::Left))
		View.Move(-20, 0);
	if(screen.GetInput().IsKeyDown(sf::Key::Up))
		View.Move(0, -20);
	if(screen.GetInput().IsKeyDown(sf::Key::Down))
		View.Move(0, 20);
	if(screen.GetInput().IsKeyDown(sf::Key::Add) 
		|| screen.GetInput().IsKeyDown(sf::Key::Equal))
		View.Zoom(1.1f);
	if(screen.GetInput().IsKeyDown(sf::Key::Subtract) 
		|| screen.GetInput().IsKeyDown(sf::Key::Dash))
		if(View.GetRect().GetWidth()/0.9 < screen.GetWidth())
			View.Zoom(0.9f);

}

bool handle_events(sf::RenderWindow &screen, sf::View &View){
	sf::Event Event;
	while(screen.GetEvent(Event)){
		//handles exits
		if(Event.Type == sf::Event::Closed)
			return true;
		if(Event.Type==sf::Event::KeyPressed && Event.Key.Code == sf::Key::Escape)
			return true;
		//handles scrolling
		if(Event.Type==sf::Event::MouseWheelMoved){
			float mod = 1;
			if(Event.MouseWheel.Delta>0)
				mod = 1+((float)Event.MouseWheel.Delta)*.1f;
			else
				mod =(float)(90+(float)Event.MouseWheel.Delta)/100.0f;
			if(View.GetRect().GetWidth()/mod < screen.GetWidth())
				View.Zoom(mod);
		}
	}
	return false;

}