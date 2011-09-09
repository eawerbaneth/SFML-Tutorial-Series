//player.cpp
#include "player.h"

//player implementation
player::player(){
	name.SetText("player1");
	//Sprite.SetColor(sf::Color::Red);
	//Sprite.Resize(100, 200);
	//Sprite.SetPosition(100.f, 600.f);
	state="NULL";
}

player::player(std::string new_name){
	Sprite.Resize(100, 200);
	name.SetText(new_name);
	sf::Image img;
	img.LoadFromFile("beth-block-bnw.png");
	for(int i=0; i<6; i++)
		states.push_back(img);
	if(new_name=="player1"){
		//Sprite.SetColor(sf::Color::Red);
		//Sprite.SetPosition(100.f, 600.f);
		Sprite.SetImage(states[0]);
		name.SetPosition(50, 50);
		health.set_pos(50.f);
	}
	else{
		//Sprite.SetColor(sf::Color::Blue);
		//Sprite.SetPosition(1180.f-Sprite.GetSize().x, 600.f);
		Sprite.SetImage(states[0]);
		name.SetPosition(800, 50);
		health.set_pos(800.f);
	}	
	state="NULL";
}

void player::destroy(){
	Sprite.Resize(1, 1);

}

sf::Vector2f player::get_pos(){
	return Sprite.GetPosition();
}

sf::Sprite player::get_Sprite(){
	return Sprite;
}

void player::adjust(float x_offset, float y_offset, sf::Vector2f enemy_pos){
	Sprite.Move(x_offset, y_offset);
	if(Sprite.GetPosition().x>enemy_pos.x)
		Sprite.FlipX(true);
	else
		Sprite.FlipX(false);
	if(Sprite.GetPosition().x<0)
		Sprite.SetX(0);
	if(Sprite.GetPosition().x+Sprite.GetSize().x>1280)
		Sprite.SetX(1280-Sprite.GetSize().x);

}

std::string player::get_state(){
	return state;
}

bool player::punch(sf::Vector2f enemy_pos, std::string enemy_state){
	//change basic stuff
	state="PUNCH";
	Sprite = sf::Sprite(states[2], Sprite.GetPosition());
	//if we're facing left, we need to adjust for that
	if(enemy_pos.x+100<Sprite.GetPosition().x+Sprite.GetSize().x){
		Sprite.FlipX(true);
		Sprite.SetPosition(Sprite.GetPosition().x-punch_offset, Sprite.GetPosition().y);
	}
	//check for collision and acceptable enemy state
	if(enemy_pos.x>=Sprite.GetPosition().x && 
		enemy_pos.x<=Sprite.GetPosition().x+Sprite.GetSize().x && 
		(enemy_state=="KICK" || enemy_state=="NULL"||enemy_state=="SPECIAL"))
		return true;
	else if(enemy_pos.x+100 >= Sprite.GetPosition().x &&
		enemy_pos.x+100 <= Sprite.GetPosition().x+Sprite.GetSize().x
		&& (enemy_state=="KICK" || enemy_state=="NULL"||enemy_state=="SPECIAL"))
		return true;
	else
		return false;

}

bool player::kick(sf::Vector2f enemy_pos, std::string enemy_state){
	state="KICK";
	Sprite = sf::Sprite(states[3], Sprite.GetPosition());
	//adjust for which way we're facing
	if(enemy_pos.x+100<Sprite.GetPosition().x+Sprite.GetSize().x){
		Sprite.SetPosition(Sprite.GetPosition().x-kick_offset, Sprite.GetPosition().y);
		Sprite.FlipX(true);
	}
	//check for collision and acceptable enemy state
	//if the enemy is to our right
	if(enemy_pos.x>=Sprite.GetPosition().x && 
		enemy_pos.x<=Sprite.GetPosition().x+Sprite.GetSize().x 
		&& (enemy_state=="BLOCK" || enemy_state=="NULL"|| enemy_state=="SPECIAL"))
		return true;
	//if the enemy is to our left
	else if(enemy_pos.x+100 >= Sprite.GetPosition().x &&
		enemy_pos.x+100 <= Sprite.GetPosition().x+Sprite.GetSize().x
		&& (enemy_state=="BLOCK" || enemy_state=="NULL"||enemy_state=="SPECIAL"))
		return true;
	else
		return false;
}

void player::block(){
	state="BLOCK";
	Sprite.SetImage(states[1]);
}

void player::prespec(sf::Vector2f enemy_pos){
	Sprite = sf::Sprite(states[4], Sprite.GetPosition());
	if(enemy_pos.x<Sprite.GetPosition().x)
		Sprite.FlipX(true);
	state="SPECIAL";
}

void player::release_state(sf::Vector2f enemy_pos){
	Sprite = sf::Sprite(states[0], Sprite.GetPosition());
	//reset position if we adjusted it
	if(enemy_pos.x<Sprite.GetPosition().x){
		Sprite.FlipX(true);
		if(state=="KICK")
			Sprite.SetPosition(Sprite.GetPosition().x+kick_offset, Sprite.GetPosition().y);
		if(state=="PUNCH")
			Sprite.SetPosition(Sprite.GetPosition().x+punch_offset, Sprite.GetPosition().y);
	}
	state="NULL";
}

bool player::take_damage(int damage, sf::Vector2f enemy_pos){
	release_state(enemy_pos);
	if(health.take_damage(damage)){
		destroy();
		return true;
	}
	return false;
}

sf::Sprite player::get_health(){
	return health.get_bar();
}

sf::String player::get_name(){
	return name;
}

special_attack* player::special(sf::Vector2f enemy_pos){
	return NULL;
}

