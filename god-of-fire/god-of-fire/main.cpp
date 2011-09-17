//main.cpp
#include <fstream>
#include "pathing.h"


sf::Image tileset;
sf::Image monk_sprites;

struct path_helper{
	int dist;
	path_helper* prev;
	sf::Vector2i pos;
};

//input utilities
bool handle_events(sf::RenderWindow &screen, sf::View &View);
void keyboard_input(sf::RenderWindow &screen, sf::View &View);
void police_boundaries(sf::View &View, sf::Vector2i &mapsize);
//game initializations
bool readmap(std::vector <std::vector<tile*>> &map);
void load_monks(std::vector <monk*> &monks, std::vector <std::vector<tile*>> &map);

//drawing utilties
void printmap(sf::RenderWindow &screen, std::vector <std::vector <tile*>> &map, std::vector <monk*> &monks);
void printmapdynamic(sf::RenderWindow &screen, std::vector <std::vector <tile*>> &map, sf::View &View, std::vector <monk*> &monks);
//void printmonks(sf::RenderWindow &screen, std::vector <monk*> &monks);
//updates
void monk_update(std::vector <std::vector <tile*>> &map, std::vector <monk*> &monks);
void find_path(std::vector <std::vector <tile*>> &map, monk* a_monk);
bool compare_function( monk* &a,  monk* &b);
bool helpersort(path_helper* &a, path_helper* &b);


int main(){
	//initalize the game window
	sf::RenderWindow screen(sf::VideoMode(800, 600), "God of Fire");
	screen.SetFramerateLimit(60);

	//fun initializations
	sf::Vector2f old_mouse_coords;
	sf::Vector2f mouse_coords;
	std::vector <std::vector <tile*>> map;
	std::vector <monk*> monks;
	int frames=0;
	//if we failed to load our map, exit program
	if(!readmap(map))
		return 0;
	load_monks(monks, map);
	
	sf::Vector2i mapsize(map[0].size()*dim, map.size()*dim/4+dim);
	
	sf::View View=screen.GetDefaultView();
	while(screen.IsOpened()){

		frames++;

		if(handle_events(screen, View))
			screen.Close();

		//mouse control
		old_mouse_coords = screen.ConvertCoords((unsigned)mouse_coords.x, (unsigned)mouse_coords.y);
		mouse_coords = screen.ConvertCoords(screen.GetInput().GetMouseX(), 
			screen.GetInput().GetMouseY());
		if(screen.GetInput().IsMouseButtonDown(sf::Mouse::Left))
			View.Move(-1.5f*(mouse_coords.x-old_mouse_coords.x), 
			-1.5f*(mouse_coords.y-old_mouse_coords.y));

		//keyboard control
		keyboard_input(screen, View);

		//maintain boundaries
		police_boundaries(View, mapsize);

		if(frames%15==0)
			monk_update(map, monks);

		//display
		screen.Clear();
		screen.SetView(View);	
		//printmap(screen, map, monks);
		printmapdynamic(screen, map, View, monks);
		//printmonks(screen, monks);
		screen.SetView(screen.GetDefaultView());
		screen.Display();

	}

	return 0;
}


void rand_dest(int &x, int &y, std::vector <std::vector <tile*>> &map){
	x = sf::Randomizer().Random(2, map.size()-3);
	y = sf::Randomizer().Random(2, map.size()-3);
	//get a random destination
	while(map[x][y]->get_type()!=' '){
		x = sf::Randomizer().Random(2, map.size()-3);
		y = sf::Randomizer().Random(2, map.size()-3);
	}
}

//update monk behavior
void monk_update(std::vector <std::vector <tile*>> &map, std::vector <monk*> &monks){
	//give each monk a chance to update himself
	for(unsigned int i=0; i<monks.size(); i++){
		//check to see if we're already at our destination
		if(monks[i]->dest_reached()){
			int x, y;
			rand_dest(x, y, map);
			monks[i]->set_dest(sf::Vector2i(x, y));
		}
		else{ //we haven't reached destination yet
			sf::Vector2i dest = monks[i]->update();
			//move if we can, if we can't: do nothing
			monks[i]->request_occupy(map[dest.x][dest.y]);
		}	
	}
}

//print map dynamically
void printmapdynamic(sf::RenderWindow &screen, std::vector <std::vector <tile*>> &map, 
	sf::View &View, std::vector <monk*> &monks){
	unsigned int mnk = 0;
	unsigned int row = (unsigned)(View.GetRect().Top)/dim;
	if(row<0) row=0;

	for(row; (row<map.size()&&row<(View.GetRect().Bottom+dim)/(dim/4)); row++){
		unsigned int col = (unsigned)(View.GetRect().Left)/dim;
		if(col<0) col=0;
		for(col; (col<map[row].size()&&col<View.GetRect().Right); col++)
			screen.Draw(map[row][col]->get_sprite());
		//finished the row, time to draw the monks
		while(mnk < monks.size() && (unsigned)monks[mnk]->get_tile().x <=row){
			if(monks[mnk]->get_tile().x == row)
				screen.Draw(monks[mnk]->get_sprite());
			mnk++;
		}
	}

}

//print entire map
void printmap(sf::RenderWindow &screen, std::vector <std::vector <tile*>> &map, std::vector <monk*> &monks){
	unsigned int mnk = 0;

	for(unsigned int i=0; i<map.size(); i++){
		for(unsigned int k=0; k<map[i].size(); k++){
			screen.Draw(map[i][k]->get_sprite());
		}
		//we've finished a row, now draw monks for that row
		while(mnk<monks.size() && (unsigned)monks[mnk]->get_tile().x <= i){
			screen.Draw(monks[mnk]->get_sprite());
			mnk++;
		}
	}
}

/*
void printmonks(sf::RenderWindow &screen, std::vector <monk*> &monks){
	for(unsigned int i=0; i<monks.size(); i++)
		screen.Draw(monks[i]->get_sprite());
}
*/

void load_monks(std::vector <monk*> &monks, std::vector <std::vector<tile*>> &map){
	int num_monks = map.size()*map[0].size()/20;
	if(!monk_sprites.LoadFromFile("imgs/monks.png"))
		return;

	for(int i=0; i<num_monks; i++){
		int randx = sf::Randomizer().Random(2, map.size()-3);
		int randy = sf::Randomizer().Random(2, map[0].size()-3);
		if(map[randx][randy]->get_type()==' ' && !map[randx][randy]->is_occupied()){
			int randx2;
			int randy2;
			//get a random destination
			rand_dest(randx2, randy2, map);
			monks.push_back(new monk(map[randx][randy], 0, &monk_sprites, map[randx2][randy2]));
			//go ahead and do pathing
			find_path(map, monks[monks.size()-1]);
		}
		else
			i--;
	}
	
	sort(monks.begin(), monks.end(), compare_function);

}

//read our file
bool readmap(std::vector <std::vector <tile*>> &map){
	//boring initializations
	std::ifstream input;
	
	if(!tileset.LoadFromFile("imgs/modtiles2.png"))
		return false;

	input.open("maps/proxy2.txt");
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

//find a monk's path to his destination (Dijkstra's Algorithm)
void find_path(std::vector <std::vector <tile*>> &map, monk* a_monk){
	sf::Clock timer;
	timer.Reset();

	std::cout << "Starting monk pathing...\n";

	sf::Vector2i dest = a_monk->get_dest();
	sf::Vector2i current = a_monk->get_tile();

	std::vector <path_helper*> helper;

	for(unsigned int i=2; i<map.size()-2; i++){
		for(unsigned int k=2; k<map[i].size()-2; k++){
			if(map[i][k]->get_type() == ' '){
				path_helper* temp = new path_helper;
				temp->pos = sf::Vector2i(i, k);
				if(temp->pos == current)
					temp->dist = 0;
				else
					temp->dist = 10000;
				temp->prev = NULL;
				helper.push_back(temp);
			}
		}
	}

	std::cout << "\t Pre-Dijkstra time: " << timer.GetElapsedTime() << "\n";

	//std::vector <path_helper> helper;

	path_helper* u;
	int iteration = 0;

	while(!helper.empty()){
		iteration++;
		std::sort(helper.begin(), helper.end(), helpersort);
		u = helper[0];
		if(u->dist == 10000)
			break; //all remaining vertices inaccessible for source
		if(u->pos == dest)
			break;
		helper.erase(helper.begin());
		for(unsigned int j=0; j<helper.size(); j++){
			//if the node is on an adjacent row
			if(helper[j]->pos.x == u->pos.x-1 || helper[j]->pos.x == u->pos.x+1){
				if(u->pos.x%2==0){//if our u is even, look at the col and col+1 on either side
					if(helper[j]->pos.y == u->pos.y || helper[j]->pos.y == u->pos.y+1){
						//we are adjacent!
						int new_dist = u->dist + 1;
						if(new_dist < helper[j]->dist){
							helper[j]->dist = new_dist;
							helper[j]->prev = u;
						}
					}
				}
				else{//we care about col-1 and col
					if(helper[j]->pos.y == u->pos.y || helper[j]->pos.y == u->pos.y-1){
						//we are adjacent!
						int new_dist = u->dist + 1;
						if(new_dist < helper[j]->dist){
							helper[j]->dist = new_dist;
							helper[j]->prev = u;
						}
					}
				}
			}
		}
		if(iteration%10==0)
			std::cout << "\tIteration " << iteration << ": " << timer.GetElapsedTime() << "\n";
	}

	std::vector <sf::Vector2i> path;

	if(u->pos == dest){
		while(u->prev != NULL){
			path.push_back(u->prev->pos);
			u=u->prev;
		}
	}

	a_monk->set_path(path);
	helper.clear();

	std::cout << "Pathing time: " << timer.GetElapsedTime() << "\n";
}

bool compare_function( monk* &a,  monk* &b){
	if(a->get_pos().y < b->get_pos().y)
		return true;
	else if(a->get_pos().y == b->get_pos().y && a->get_pos().x < b->get_pos().x)
		return true;
	else
		return false;
}

bool helpersort(path_helper* &a, path_helper* &b){
	return a->dist < b->dist;
}

