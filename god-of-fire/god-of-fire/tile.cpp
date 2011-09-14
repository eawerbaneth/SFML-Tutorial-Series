#include "tile.h"

tile::tile(int row, int col, int Z, char Type, sf::Image *Image){
	z=Z;
	type=Type;
	corrupted=false;
	Sprite.SetImage(*Image);
	int xmod=0;

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

monk::monk(tile* tile, int Type, sf::Image *Image){
	type = Type;
	Sprite.SetImage(*Image);
	int xmod=0;

	Sprite.SetSubRect(sf::IntRect(xmod*citizen_dim, Image->GetHeight(), 
		xmod*citizen_dim+citizen_dim, Image->GetHeight()));

	Sprite.SetPosition(tile->get_pos().x+(dim/2-Sprite.GetSubRect().GetWidth()/2),
		tile->get_pos().y+(dim/8));

}
