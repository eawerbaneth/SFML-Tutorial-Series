#include "tile.h"

bool helpersort2(path_helper* &a, path_helper* &b){
	return a->dist < b->dist;
}

tile::tile(int row, int col, int Z, char Type, sf::Image *Image){
	z=Z;
	type=Type;
	corrupted=false;
	Sprite.SetImage(*Image);
	int xmod=0;
	coords = sf::Vector2i(row, col);
	occupied = false;
	ignited = false;

	//wall
	if(type == 'X'){ xmod = 2; z=3;}
	//water
	else if(type == 'O') xmod = 3;
	//neutral
	else if(type == ' '){ xmod = 1; z=1;}

	Sprite.SetSubRect(sf::IntRect(xmod*dim, z*dim, (xmod + 1)*dim, (z + 1)*dim));

	if(row%2==0)
		Sprite.SetPosition((float)col*dim, (float)row*dim/4);
	else
		Sprite.SetPosition((float)col*dim-dim/2, (float)row*dim/4);

}

//respond to a monk trying to move onto us (should only be called from monk)
bool tile::occupy(){
	if(occupied == false && type == ' '){
		occupied = true;
		return true;
	}
	else
		return false;
}

bool tile::ignite(){
	//if we're looking at a water tile or a wall tile, we can't set it on fire
	if(type != ' ')
		return false;
	//if it's already on fire, we can't re-ignite it
	if(ignited)
		return false;

	//go ahead and set this guy on fire
	ignited = true;
	int xmod = 4;
	Sprite.SetSubRect(sf::IntRect(xmod*dim, z*dim, (xmod + 1)*dim, (z + 1)*dim));
	firedur = 0;

	return true;
}

//monk implementation
monk::monk(tile* my_tile, sf::Image *Image, tile* destination){
	Sprite.SetImage(*Image);
	int xmod=0;
	tilecoords = my_tile->get_coords();
	destcoords = destination->get_coords();

	Sprite.SetSubRect(sf::IntRect(xmod*citizen_dim, 0, 
		xmod*citizen_dim+citizen_dim, Image->GetHeight()));

	Sprite.SetPosition(my_tile->get_pos().x+(dim/2-Sprite.GetSubRect().GetWidth()/2),
		my_tile->get_pos().y+(dim/2-Sprite.GetSubRect().GetHeight()));

}

//try to move onto a new tile
bool monk::request_occupy(tile* new_tile){
	//if(new_tile->occupy()){
	Sprite.SetPosition(new_tile->get_pos().x+(dim/2-Sprite.GetSubRect().GetWidth()/2),
		new_tile->get_pos().y+(dim/2-Sprite.GetSubRect().GetHeight()));
	tilecoords = new_tile->get_coords();
	if(!path.empty())
		path.pop_back();
	return true;
	//}
	//else
	//	return false;
}

sf::Vector2i monk::update(){
	if(!path.empty())
		return path[path.size()-1];
	else
		return destcoords;
}


//faithful implementation
faithful::faithful(tile* my_tile, sf::Image *Image, tile* destination){
	Sprite.SetImage(*Image);
	tilecoords = my_tile->get_coords();
	destcoords = destination->get_coords();
	selected = false;
	detonated = false;
	death_walk = 0;
	my_Image = Image;

	Sprite.SetSubRect(sf::IntRect(2*citizen_dim, 0, 2*citizen_dim+citizen_dim, 
		Image->GetHeight()));

	Sprite.SetPosition(my_tile->get_pos().x+(dim/2-Sprite.GetSubRect().GetWidth()/2),
		my_tile->get_pos().y+(dim/2-Sprite.GetSubRect().GetHeight()));

}

faithful::faithful(monk* old_monk, sf::Image *Image){
	Sprite = old_monk->get_sprite();
	tilecoords = old_monk->get_tile();
	destcoords = old_monk->get_dest();
	selected = false;
	detonated = false;
	death_walk = 0;
	my_Image = Image;

	Sprite.SetSubRect(sf::IntRect(2*citizen_dim, 0, 2*citizen_dim+citizen_dim, 
		my_Image->GetHeight()));

	Sprite.SetPosition(old_monk->get_pos());

}

std::vector <sf::Vector2i> faithful::get_range(std::vector <std::vector <tile*>> &map){
	//we're going to store everything within <allowance> tiles in this vector
	int allowance = 3;
	std::vector <sf::Vector2i> range;

	//if this monk hasn't been selected, we're not going to let main see its range
	if(!selected)
		return range;

	std::vector <path_helper*> helper;

	//we don't really need to check our entire map for this range
	int row = tilecoords.y - 5;
	if(row  < 0) row = 0;
	int col = tilecoords.x - 5;
	if(col < 0) col = 0;

	//we're only going to be looking at a subset of our map for this
	for(row; row<(signed)map.size()-2 && row<tilecoords.y+5; row++){
		for(col; col<(signed)map[row].size()-2 && col<tilecoords.x+5; col++){
			if(map[row][col]->get_type() == ' '){
				path_helper* temp = new path_helper;
				temp->pos = sf::Vector2i(row, col);
				if(temp->pos == tilecoords)
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
		std::sort(helper.begin(), helper.end(), helpersort2);
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

	deto_range = range;
	return range;

}

//we're going to entrust the task of enforcing one selected monk at a time to main
bool faithful::select(){
	//if the monk has already detonated himself, don't let the user select him
	if(!detonated){
		if(!selected){
			selected = true;
			Sprite.SetSubRect(sf::IntRect(3*citizen_dim, 0, 3*citizen_dim+citizen_dim, 
				my_Image->GetHeight()));
			return true;
		}
		//allow the user to select of deselect
		else{
			selected = false;
			Sprite.SetSubRect(sf::IntRect(2*citizen_dim, 0, 2*citizen_dim+citizen_dim, 
				my_Image->GetHeight()));
			return false;
		}
	}
	//couldn't be selected, sends the signal to highlight
	return false;
}

bool faithful::detonate(tile* target){
	if(selected == true){
		//make sure the target is within range
		bool found = false;
		for(unsigned int i=0; i<deto_range.size(); i++)
			if(deto_range[i]==target->get_coords())
				found = true;

		//if it was, then detonate and and set destination
		if(found){
			detonated = true;
			destcoords = target->get_coords();
			Sprite.SetSubRect(sf::IntRect(4*citizen_dim, 0, 4*citizen_dim+citizen_dim, 
				my_Image->GetHeight()));
			return true;
		}
	}
	return false;
}

bool faithful::update(std::vector <std::vector <tile*>> &map){
	//first check whether or not the monk has been detonated
	if(detonated){
		//check to see if it's time for us to die
		if(death_walk>3)
			return true;
		//we're going to set our current tile on fire
		map[tilecoords.x][tilecoords.y]->ignite();
		death_walk++;
		return false;
	}
	//we're in standby, do nothing
	return false;
}

//corrupted implementation
corrupted::corrupted(tile* my_tile, sf::Image *Image, tile* destination){
	Sprite.SetImage(*Image);
	int xmod=1;
	tilecoords = my_tile->get_coords();
	destcoords = destination->get_coords();

	Sprite.SetSubRect(sf::IntRect(xmod*citizen_dim, 0, 
		xmod*citizen_dim+citizen_dim, Image->GetHeight()));

	Sprite.SetPosition(my_tile->get_pos().x+(dim/2-Sprite.GetSubRect().GetWidth()/2),
		my_tile->get_pos().y+(dim/2-Sprite.GetSubRect().GetHeight()));

}

