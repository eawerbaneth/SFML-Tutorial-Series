#include "misc.h"	

health_bar::health_bar(){
	health=100;
	bar.Resize(400, 50);
	bar.SetColor(sf::Color::White);
}

//returns true if player dies
bool health_bar::take_damage(int damage){
	health-=damage;
	if(health>0){
		bar.Resize((float)health*4, 50);
		return false;
	}
	else{
		bar.Resize(1, 1);
		return true;
	}
}

sf::Sprite health_bar::get_bar(){
	return bar;
}

void health_bar::set_pos(float x){
	bar.SetPosition(x, 100.f);
}

//SPECIAL ATTACKS
special_attack::special_attack(){}

blast::blast(){}

blast::blast(sf::Vector2f pos, bool dir){
	image.SetPosition(pos);
	direction=dir;
	image.Resize(50, 50);
	image.SetColor(sf::Color::Cyan);
}

blast::~blast(){}

sf::Sprite blast::get_image(float ElapsedTime, bool &alive){
	alive=advance(ElapsedTime);
	return image;
}

bool blast::advance(float ElapsedTime){
	//if the blast is going left
	if(direction==false){
		image.Move(-300*ElapsedTime, 0);
		if(image.GetPosition().x<0)
			return false;
	}
	//blast is going right
	else{
		image.Move(300*ElapsedTime, 0);
		if(image.GetPosition().x>1280)
			return false;
	}
	return true;
}

bool blast::collides(sf::Vector2f enemy_pos, sf::Vector2f enemy_size, std::string enemy_state){
	//enemy is to the left and we've collided
	if(enemy_pos.x+enemy_size.x>=image.GetPosition().x&&
		enemy_pos.x+enemy_size.x<=image.GetPosition().x+image.GetSize().x){
		//they're parrying it with a punch
		if(enemy_state=="PUNCH"){
			direction=true;
			return false;
		}
		//in all other cases, they get damaged
		return true;
	}
	//enemy is to the right and we've collided
	else if(enemy_pos.x>=image.GetPosition().x+image.GetSize().x&&
		enemy_pos.x<=image.GetPosition().x){
		//parried with a punch
		if(enemy_state=="PUNCH"){
			direction=false;
			return false;
		}
		//in all other cases, they get damaged
		return true;
	}
	else{
		return false;
	}
}
