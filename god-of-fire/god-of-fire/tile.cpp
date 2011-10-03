#include "tile.h"

tile::tile(int row, int col, int Z, char Type, sf::Image *Image){
	z=Z;
	type=Type;
	corrupted=false;
	Sprite.SetImage(*Image);
	int xmod=0;
	coords = sf::Vector2i(row, col);
	occupied = false;
	ignited = false;
	highlighted = false;
	invulnerable = 0;

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

//let the motherf***er burn
bool tile::ignite(){
	//clear out corruption
	corrupted = false;

	//if the tile is on cooldown, we're not going to touch it
	if(invulnerable>0)
		return false;
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
	//firedur = 0;

	return true;
}

void tile::extinguish(){
	ignited = false;
	int xmod = 1;
	Sprite.SetSubRect(sf::IntRect(xmod*dim, z*dim, (xmod + 1)*dim, (z + 1)*dim));

}

//highlight the tile
void tile::highlight(){
	//we're not going to highlight walls or water
	if(type == 'X' || type == 'O') return;

	int xmod;
	//if it's already been highlighted, unhighlight it
	if(highlighted){
		highlighted=false;
		xmod = 1;
	}
	//highlight it
	else{
		highlighted = true;
		xmod = 5;
	}
	Sprite.SetSubRect(sf::IntRect(xmod*dim, z*dim, (xmod + 1)*dim, (z + 1)*dim));
}

//corrupt the tile
void tile::corrupt(){
	//we're not going to corrupt walls or water (for now)
	if(type != ' ') return;
	//if it's burning, we can't corrupt it
	if(ignited) return;
	//if it's on cooldown, leave it alone
	if(invulnerable > 0) return;

	corruption_cooldown = 0;
	corrupted = true;
	int xmod = 0;
	Sprite.SetSubRect(sf::IntRect(xmod*dim, z*dim, (xmod + 1)*dim, (z+1)*dim));

}

void tile::update(std::vector <std::vector<tile*>> &map){
	//if tile is corrupted, spread the corruption after a few turns
	if(corrupted){
		if(corruption_cooldown > 2){
			if(coords.x>0)
				map[coords.x-1][coords.y]->corrupt();
			if(coords.x<(signed)map.size()-1)
				map[coords.x+1][coords.y]->corrupt();
			//even row, look at col+1
			if(coords.x%2==0 && coords.y<(signed)map[coords.x].size()-1){
				if(coords.x>0)
					map[coords.x-1][coords.y+1]->corrupt();
				if(coords.x<(signed)map.size()-1)
					map[coords.x+1][coords.y+1]->corrupt();
			}
			//odd row, look at col and col-1
			else if(coords.y > 0){
				if(coords.x>0)
					map[coords.x-1][coords.y-1]->corrupt();
				if(coords.x<(signed)map.size()-1)
					map[coords.x+1][coords.y-1]->corrupt();
			}
			corrupted = false;
		}
		corruption_cooldown++;
	}
	invulnerable--;

}

