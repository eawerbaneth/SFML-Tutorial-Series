#include "tile.h"

tile::tile(int row, int col, int Z, char Type, sf::Image *Image){
	z=Z;
	type=Type;
	corrupted=false;
	Sprite.SetImage(*Image);
	int xmod=0;
	coords = sf::Vector2i(row, col);
	occupied = false;

	//wall
	if(type == 'X'){ xmod = 2; z=3;}
	//water
	else if(type == 'O') xmod = 3;
	//neutral
	else if(type == ' '){ xmod = 1; z=1;}

	Sprite.SetSubRect(sf::IntRect(xmod*dim, z*dim, (xmod + 1)*dim, (z + 1)*dim));

	if(row%2==0)
		Sprite.SetPosition(col*dim, row*dim/4);
	else
		Sprite.SetPosition(col*dim-dim/2, row*dim/4);

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

monk::monk(tile* my_tile, int Type, sf::Image *Image, tile* destination){
	type = Type;
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
	if(new_tile->occupy()){
		Sprite.SetPosition(new_tile->get_pos().x+(dim/2-Sprite.GetSubRect().GetWidth()/2),
			new_tile->get_pos().y+(dim/2-Sprite.GetSubRect().GetHeight()));
		tilecoords = new_tile->get_coords();
		if(!path.empty())
			path.pop_back();
		return true;
	}
	else
		return false;
}

sf::Vector2i monk::update(){
	if(!path.empty())
		return path[path.size()-1];
	else
		return destcoords;
}