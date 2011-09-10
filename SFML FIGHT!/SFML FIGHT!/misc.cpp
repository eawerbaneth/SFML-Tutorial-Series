#include "misc.h"
#include <cmath>


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

//blast
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
		image.Move(-500*ElapsedTime, 0);
		if(image.GetPosition().x<0)
			return false;
	}
	//blast is going right
	else{
		image.Move(500*ElapsedTime, 0);
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
		//or they just blocked it
		if(enemy_state=="BLOCK")
			return false;
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
		//or they just blocked it
		if(enemy_state=="BLOCK")
			return false;
		//in all other cases, they get damaged
		return true;
	}
	else{
		return false;
	}
}

//hawk

hawk::hawk(){}

hawk::hawk(sf::Vector2f my_pos, sf::Vector2f enemy_pos){
	//initialize sprite
	image.Resize(50, 50);
	image.SetColor(sf::Color::Red);
	//check which direction first sweep is going to be
	if(my_pos.x<enemy_pos.x)//enemy is to our right
		generate_parabola(enemy_pos, true);
	else
		generate_parabola(enemy_pos, false);
	image.SetPosition(path[0]);
	path.erase(path.begin());
}

hawk::~hawk(){}

sf::Sprite hawk::get_image(float ElapsedTime, bool &alive){
	alive=advance(ElapsedTime);
	return image;
}

bool hawk::collides(sf::Vector2f enemy_pos, sf::Vector2f enemy_size, std::string enemy_state){
	//debugging
	//return false;
	//enemy is to the left and we've collided
	if(enemy_pos.x+enemy_size.x>=image.GetPosition().x&&
		enemy_pos.x+enemy_size.x<=image.GetPosition().x+image.GetSize().x
		&&enemy_pos.y<=image.GetPosition().y+image.GetSize().y){
		//they're parrying it with a punch
		if(enemy_state=="BLOCK"){
			return false;
		}
		//in all other cases, they get damaged
		return true;
	}
	//enemy is to the right and we've collided
	else if(enemy_pos.x>=image.GetPosition().x+image.GetSize().x&&
		enemy_pos.x<=image.GetPosition().x
		&&enemy_pos.y<=image.GetPosition().y+image.GetSize().y){
		//blocked
		if(enemy_state=="BLOCK"){
			return false;
		}
		//in all other cases, they get damaged
		return true;
	}
	//we didn't collide at all
	else{
		return false;
	}
}

void hawk::generate_parabola(sf::Vector2f enemy_pos, bool dir){
	if(dir==true){
		for(float i=-300; i<=300; i+=150){
			float y=600-((i*i)/500);
			path.push_back(sf::Vector2f(enemy_pos.x+i, y));
		}
	}
	else{
		for(float i=300; i>=-300; i-=150){
			float y=600-((i*i)/500);
			path.push_back(sf::Vector2f(enemy_pos.x+i, y));
		}
	}

}

float get_dist(sf::Vector2f p1, sf::Vector2f p2){
	return sqrt(pow(p1.x-p2.x, 2)+pow(p1.y-p2.y, 2));
}

float get_angle(sf::Vector2f p1, sf::Vector2f p2){
	float angle= atan2((p1.y-p2.y), (p1.x-p2.x));
	return PI+angle;

}

bool hawk::advance(float ElapsedTime){
	if(path.size()==0)
		return false;
	//move towards path[0]
	else{
		//determine angle we want to move to
		float angle=get_angle(image.GetPosition(), path[0]);
		//move as far as we can this frame
		old_pos= image.GetPosition();
		image.Move(300*cos(angle)*ElapsedTime, 300*sin(angle)*ElapsedTime);
		//if we're close enough, clear the checkpoint
		if(get_dist(image.GetPosition(), path[0])>get_dist(old_pos, path[0]))
			path.erase(path.begin());
	}
	return true;
}

