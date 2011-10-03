#include "tile.h"

//help with pathing
bool helpersort3(path_helper* &a, path_helper* &b){
	return a->dist < b->dist;
}


void fire::update(std::vector<std::vector<tile*>> &map){
	for(unsigned int i=0; i<fires.size(); i++){
		if(!fires[i]->update(map)){
			fires.erase(fires.begin()+i);
			i--;
		}
	}
}

void fire::start_fire(sf::Vector2i source, std::vector<std::vector<tile*>> &map){
	fires.push_back(new fire_source(source, 3, map));
}


//find out how far this fire can spread
void fire_source::set_range(int allowance, 
		std::vector <std::vector <tile*>> &map){
	
	std::vector <path_helper*> helper;

	//we don't really need to check our entire map for this range
	int row = source.x - allowance - 2;
	if(row  < 0) row = 0;

	//we're only going to be looking at a subset of our map for this
	for(row; row<(signed)map.size()-2 && row<source.x+ allowance + 2; row++){
		int col = source.y - allowance - 2;
		if(col < 0) col = 0;
		for(col;col<(signed)map[row].size()-2&&col<source.y+allowance+2;col++){
			if(map[row][col]->get_type() == ' '){
				path_helper* temp = new path_helper;
				temp->pos = sf::Vector2i(row, col);
				if(temp->pos == source)
					temp->dist = 0;
				else
					temp->dist = 10000;
				temp->prev = NULL;
				helper.push_back(temp);
			}
		}
	}

	path_helper* u;

	while(!helper.empty()){
		std::sort(helper.begin(), helper.end(), helpersort3);
		u = helper[0];
		if(u->dist == 10000)
			break; //all remaining vertices inaccessible for source
		if(u->dist<=allowance)
			range.push_back(u->pos);
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
	}
}

//fire source constructor
fire_source::fire_source(sf::Vector2i the_source, int new_range, 
			std::vector <std::vector <tile*>> &map){
	source = the_source;
	set_range(new_range, map);
	//add source to active nodes
	nodes.push_back(new fire_node(the_source));
	//erase source from inactive nodes
	range.erase(range.begin());
}

//check to see if two nodes are adjacent
bool is_adjacent(sf::Vector2i source, sf::Vector2i comp){
	if(comp.x==source.x-1 && comp.y==source.y)
			return true;
	if(comp.x==source.x+1 && comp.y==source.y)
			return true;
	if(source.x%2==0){
		if(comp.x == source.x-1 && comp.y == source.y+1)
			return true;
		if(comp.x == source.x+1 && comp.y == source.y+1)
			return true;
	}
	else{
		if(comp.x==source.x-1 && comp.y==source.y -1)
			return true;
		if(comp.x==source.x+1 && comp.y==source.y-1)
			return true;
	}
	return false;
}

//update the local fire
bool fire_source::update(std::vector <std::vector <tile*>> &map){
	for(unsigned int i=0; i<nodes.size(); i++){
		if(nodes[i]->update()){
			//spread the fire if we can
			if(nodes[i]->duration==1){
				for(unsigned int k=0; k<range.size(); k++){
					//if the node is adjacent
					if(is_adjacent(nodes[i]->pos, range[k])){
						//set it on fire and move it to active
						map[range[k].x][range[k].y]->ignite();
						nodes.push_back(new fire_node(range[k]));
						range.erase(range.begin()+k);
						k--;
					}
				}
			}
		}
		//remove the node from actives
		else{
			map[nodes[i]->pos.x][nodes[i]->pos.y]->extinguish();
			nodes.erase(nodes.begin()+i);
			i--;
		}
	}

	if(nodes.empty())
		return false;
	else
		return true;
}

//fire node constructor
fire_node::fire_node(sf::Vector2i new_pos){
	pos = new_pos;
	duration = 3;
}

//update single node (false if expired)
bool fire_node::update(){
	if(duration<0)
		return false;
	duration--;
	return true;
}

/*
//invul constructor
invul::invul(sf::Vector2i new_pos, tile* new_tile){
	pos=new_pos;
	duration=3;
}

bool invul::update(){
	if(duration<0)
		return false;
	duration--;
	return true;
}
*/