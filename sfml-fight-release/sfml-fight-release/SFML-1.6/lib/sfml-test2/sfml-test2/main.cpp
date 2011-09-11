#include <sstream>
#include <fstream>
#include <time.h>
#include <vector>
#include <cmath>


//health_bar.h
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

const float PI = 3.14159265;

class health_bar{
public:
	health_bar();
	bool take_damage(int damage);
	sf::Sprite get_bar();
	void set_pos(float x);

private:
	sf::Sprite bar;
	int health;

};

//Special Attacks
class special_attack{
public:
	special_attack();
	virtual sf::Sprite get_image(float ElapsedTime, bool &alive)=0;
	virtual bool collides(sf::Vector2f enemy_pos, sf::Vector2f enemy_size, 
		std::string enemy_state)=0;
};

//Beth's Special
class blast: public special_attack{
public:
	blast();
	blast(sf::Vector2f pos, bool dir);
	~blast();
	sf::Sprite get_image(float ElapsedTime, bool &alive);
	bool collides(sf::Vector2f enemy_pos, sf::Vector2f enemy_size, std::string enemy_state);

private:
	sf::Sprite image;
	bool direction;
	bool advance(float ElapsedTime);
};

class hawk: public special_attack{
public:
	hawk();
	hawk(sf::Vector2f my_pos, sf::Vector2f enemy_pos);
	~hawk();
	sf::Sprite get_image(float ElapsedTime, bool &alive);
	bool collides(sf::Vector2f enemy_pos, sf::Vector2f enemy_size, std::string enemy_state);

private:
	sf::Sprite image;
	std::vector <sf::Vector2f> path;
	sf::Vector2f old_pos;
	void generate_parabola(sf::Vector2f enemy_pos, bool dir);
	bool advance(float ElapsedTime);
};




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






class player{
public:
	//consturctors
	player();
	player(std::string new_name);

	//movement utilities
	void adjust(float x_offset, float y_offset, sf::Vector2f enemy_pos);

	//attack utilities
	bool punch(sf::Vector2f enemy_pos, std::string enemy_state);
	bool kick(sf::Vector2f enemy_pos, std::string enemy_state);
	void block();
	void release_state(sf::Vector2f enemy_pos);
	void prespec(sf::Vector2f enemy_pos);
	virtual special_attack* special(sf::Vector2f enemy_pos);

	//accessing privates
	std::string get_state();
	sf::Vector2f get_pos();
	bool take_damage(int damage, sf::Vector2f enemy_pos);
	bool alive();

	//drawing utility
	sf::Sprite get_Sprite();
	sf::String get_name();
	sf::Sprite get_health();

protected:
	void destroy();
	sf::Sprite Sprite;
	std::string state;
	sf::String name;
	health_bar health;
	std::vector <sf::Image> states;
	float kick_offset;
	float punch_offset;
};

//SPECIALIZED CHARACTERS
class FIGHTBETH : public player{
public:
	FIGHTBETH();
	FIGHTBETH(std::string pos);
	blast *special(sf::Vector2f enemy_pos);

};

class FIGHTDAN : public player{
public:
	FIGHTDAN();
	FIGHTDAN(std::string pos);
	hawk *special(sf::Vector2f enemy_pos); 
	
};



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



//SPECIFIC FIGHTER IMPLEMENTATION
FIGHTBETH::FIGHTBETH(){
	Sprite.Resize(100, 200);
	name.SetText("BETH");
	Sprite.SetColor(sf::Color::Red);
	Sprite.SetPosition(100.f, 600.f);
	name.SetPosition(50, 50);
	health.set_pos(50.f);
	state="NULL";
}

FIGHTBETH::FIGHTBETH(std::string pos){
	name.SetText("BETH");
	//store all of Beth's visual states
	sf::Image img;
	img.LoadFromFile("imgs/BETH/beth-idle.png");
	states.push_back(img);
	img.LoadFromFile("imgs/BETH/beth-block.png");
	states.push_back(img);
	img.LoadFromFile("imgs/BETH/beth-punch.png");
	states.push_back(img);
	img.LoadFromFile("imgs/BETH/beth-kick.png");
	states.push_back(img);
	img.LoadFromFile("imgs/BETH/beth-prespec.png");
	states.push_back(img);
	img.LoadFromFile("imgs/BETH/beth-spec.png");
	states.push_back(img);

	Sprite.SetImage(states[0]);

	if(pos=="player1"){
		Sprite.SetPosition(100.f, 800.f-Sprite.GetSize().y);
		name.SetPosition(50, 50);
		health.set_pos(50.f);
	}
	else{
		Sprite.FlipX(true);
		Sprite.SetPosition(1180.f-Sprite.GetSize().x, 800.f-Sprite.GetSize().y);
		name.SetPosition(800, 50);
		health.set_pos(800.f);
	}	
	state="NULL";

	kick_offset = (float)states[3].GetWidth()-states[0].GetWidth();
	punch_offset = (float)states[2].GetWidth()-states[0].GetWidth();

}

blast* FIGHTBETH::special(sf::Vector2f enemy_pos){
	//Sprite.SetColor(sf::Color::White);
	Sprite.SetImage(states[5]);
	if(Sprite.GetPosition().x<enemy_pos.x)
		return new blast(sf::Vector2f(Sprite.GetPosition().x+Sprite.GetSize().x, 
			Sprite.GetPosition().y+(Sprite.GetSize().y/2)), true);
	else
		return new blast(sf::Vector2f(Sprite.GetPosition().x, 
			Sprite.GetPosition().y+Sprite.GetSize().y/2), false);
}

FIGHTDAN::FIGHTDAN(){}

FIGHTDAN::FIGHTDAN(std::string pos){
	name.SetText("DAN");
	//store all of Beth's visual states
	sf::Image img;
	img.LoadFromFile("imgs/DAN/dan-idle.png");
	states.push_back(img);
	img.LoadFromFile("imgs/DAN/dan-block.png");
	states.push_back(img);
	img.LoadFromFile("imgs/DAN/dan-punch.png");
	states.push_back(img);
	img.LoadFromFile("imgs/DAN/dan-kick.png");
	states.push_back(img);
	img.LoadFromFile("imgs/DAN/dan-prespec.png");
	states.push_back(img);
	img.LoadFromFile("imgs/DAN/dan-spec.png");
	states.push_back(img);

	Sprite.SetImage(states[0]);

	if(pos=="player1"){
		Sprite.SetPosition(100.f, 800.f-Sprite.GetSize().y);
		name.SetPosition(50, 50);
		health.set_pos(50.f);
	}
	else{
		Sprite.FlipX(true);
		Sprite.SetPosition(1180.f-Sprite.GetSize().x, 800.f-Sprite.GetSize().y);
		name.SetPosition(800, 50);
		health.set_pos(800.f);
	}	
	state="NULL";

	kick_offset = (float)states[3].GetWidth()-states[0].GetWidth();
	punch_offset = (float)states[2].GetWidth()-states[0].GetWidth();
}

hawk *FIGHTDAN::special(sf::Vector2f enemy_pos){
	state="SPECIAL";
	Sprite = sf::Sprite(states[5], Sprite.GetPosition());
	if(enemy_pos.x<Sprite.GetPosition().x)
		Sprite.FlipX(true);
	return new hawk(Sprite.GetPosition(), enemy_pos);
}

//prototypes
bool again(sf::RenderWindow &App, bool p1alive, bool p2alive);
void handle_countdown(sf::RenderWindow &App);
void welcome(sf::RenderWindow &App);
void moveto(sf::Sprite &Sprite, sf::Vector2f dest, float elapsed);
player* character_select(sf::RenderWindow &App, std::string pos);

int main(){

	//for now I'm going to keep it in development mode for debugging purposes
	std::cout << "Development Mode:\n";

	//initialize the game window
	sf::RenderWindow App(sf::VideoMode(1280, 800), "SFML FIGHT!");

	//opening interfaces
	welcome(App);

	bool play_again=true;

	while(play_again&&App.IsOpened()){

		player* player_1 = character_select(App, "player1");
		player* player_2 = character_select(App, "player2");

		//initialize game utilities
		bool p1lock=false, p2lock=false;
		bool p1_block=false, p2_block=false;
		bool p1_special=false, p2_special=false;
		sf::Clock p1Clock, p2Clock;
		sf::Clock timer;
		bool game_over=false;
		special_attack* p1spec;
		special_attack* p2spec;



		//countdown!
		handle_countdown(App);

		//GAME LOOP
		timer.Reset();
		while(App.IsOpened()/*&&timer.GetElapsedTime()<60*/&&game_over==false){

			//EVENT HANDLING
			sf::Event Event;
			while(App.GetEvent(Event)){
				if(Event.Type==sf::Event::Closed)
					App.Close();
				if(Event.Type==sf::Event::KeyPressed && Event.Key.Code == sf::Key::Escape)
					App.Close();
			}

			//MAINTAIN CONSISTENCY
			float ElapsedTime = App.GetFrameTime();

			//PLAYER ONE CONTROLS
			if(p1lock==false){
				//movement
				if(App.GetInput().IsKeyDown(sf::Key::A))
					player_1->adjust(-150*ElapsedTime, 0, player_2->get_pos());
				if(App.GetInput().IsKeyDown(sf::Key::D))
					player_1->adjust(150*ElapsedTime, 0, player_2->get_pos());
				//punch
				if(App.GetInput().IsKeyDown(sf::Key::G)){
					if(player_1->punch(player_2->get_pos(), player_2->get_state())){
						game_over=player_2->take_damage(10, player_1->get_pos());
					}
					p1lock=true;
					p1Clock.Reset();
				}
				//kick
				else if(App.GetInput().IsKeyDown(sf::Key::V)){
					if(player_1->kick(player_2->get_pos(), player_2->get_state()))
						game_over=player_2->take_damage(25, player_1->get_pos());
					p1lock=true;
					p1Clock.Reset();
				}
				//special
				else if(App.GetInput().IsKeyDown(sf::Key::B)){
					player_1->prespec(player_2->get_pos());
					p1lock=true;
					p1Clock.Reset();
				}
				//block
				else if(App.GetInput().IsKeyDown(sf::Key::H)&&p1_block==false){
					player_1->block();
					p1_block=true;
				}
			}
			//handle unlocking the player
			else{
				if(player_1->get_state()=="NULL")
					p1lock=false;
				else if(player_1->get_state()=="KICK"&&p1Clock.GetElapsedTime()>=.75){
					player_1->release_state(player_2->get_pos());
					p1lock=false;
				}
				else if(player_1->get_state()=="PUNCH"&&p1Clock.GetElapsedTime()>=0.5){
					player_1->release_state(player_2->get_pos());
					p1lock=false;
				}
				else if(player_1->get_state()=="SPECIAL"&&p1Clock.GetElapsedTime()>=1.5
					&&p1Clock.GetElapsedTime()<2){
						p1spec=player_1->special(player_2->get_pos());
						p1_special=true; 
				}
				else if(player_1->get_state()=="SPECIAL"&&p1Clock.GetElapsedTime()>=2){
					player_1->release_state(player_2->get_pos());
					p1lock=false;
				}
			}
			//handle unblocking
			if(p1_block==true){
				if(!App.GetInput().IsKeyDown(sf::Key::H)){
					player_1->release_state(player_2->get_pos());
					p1_block=false;
				}
			}

			//PLAYER TWO CONTROLS
			if(p2lock==false){
				//movement
				if(App.GetInput().IsKeyDown(sf::Key::Left))
					player_2->adjust(-150*ElapsedTime, 0, player_1->get_pos());
				if(App.GetInput().IsKeyDown(sf::Key::Right))
					player_2->adjust(150*ElapsedTime, 0, player_1->get_pos());
				//punch
				if(App.GetInput().IsKeyDown(sf::Key::SemiColon)){
					if(player_2->punch(player_1->get_pos(), player_1->get_state()))
						game_over=player_1->take_damage(10, player_2->get_pos());
					p2lock=true;
					p2Clock.Reset();
				}
				//kick
				else if(App.GetInput().IsKeyDown(sf::Key::Period)){
					if(player_2->kick(player_1->get_pos(), player_1->get_state()))
						game_over=player_1->take_damage(25, player_2->get_pos());
					p2lock=true;
					p2Clock.Reset();
				}
				//special
				else if(App.GetInput().IsKeyDown(sf::Key::Slash)){
					player_2->prespec(player_1->get_pos());
					p2lock=true;
					p2Clock.Reset();
				}
				//block
				else if(App.GetInput().IsKeyDown(sf::Key::Quote)){
					player_2->block();
					p2_block=true;
				}
			}
			else{
				if(player_2->get_state()=="NULL")
					p2lock=false;
				else if(player_2->get_state()=="KICK"&&p2Clock.GetElapsedTime()>=0.5){
					player_2->release_state(player_1->get_pos());
					p2lock=false;
				}
				else if(player_2->get_state()=="PUNCH"&&p2Clock.GetElapsedTime()>=0.25){
					player_2->release_state(player_1->get_pos());
					p2lock=false;
				}
				else if(player_2->get_state()=="SPECIAL"&&p2Clock.GetElapsedTime()>=1.5
					&&p2Clock.GetElapsedTime()<2){
						p2spec=player_2->special(player_1->get_pos());
						p2_special=true; 
				}
				else if(player_2->get_state()=="SPECIAL"&&p2Clock.GetElapsedTime()>=2){
					player_2->release_state(player_1->get_pos());
					p2lock=false;
				}
			}
			//handle unblocking
			if(p2_block==true){
				if(!App.GetInput().IsKeyDown(sf::Key::Quote)){
					player_2->release_state(player_1->get_pos());
					p2_block=false;
				}
			}

			std::ostringstream buff;
			buff << 60-(int)timer.GetElapsedTime();
			sf::String time(buff.str());
			time.SetPosition((float)App.GetWidth()/2, 100);

			//DRAWING THE FRAME
			App.Clear();
			//top interface
			App.Draw(time);
			App.Draw(player_1->get_name());
			App.Draw(player_2->get_name());
			App.Draw(player_1->get_health());
			App.Draw(player_2->get_health());
			//drawing specials
			if(p1_special){
				App.Draw(p1spec->get_image(ElapsedTime, p1_special));
				if(p1spec->collides(player_2->get_pos(), player_2->get_Sprite().GetSize(), 
					player_2->get_state())){
						game_over=player_2->take_damage(40, player_1->get_pos());
						p1_special=false;
				}
			}
			if(p2_special){
				App.Draw(p2spec->get_image(ElapsedTime, p2_special));
				if(p2spec->collides(player_1->get_pos(), player_1->get_Sprite().GetSize(), 
					player_1->get_state())){
						game_over=player_1->take_damage(40, player_2->get_pos());
						p2_special=false;
				}
			}
			//drawing characters
			App.Draw(player_1->get_Sprite());
			App.Draw(player_2->get_Sprite());
			App.Display();

		}

		play_again=again(App, player_1->take_damage(0, player_2->get_pos()), 
			player_2->take_damage(0, player_1->get_pos()));

	}

	return 0;
}

bool again(sf::RenderWindow &App, bool p1alive, bool p2alive){
	sf::Clock timer;
	//play again dialogue
	timer.Reset();
	sf::String dialogue;
	//take damage returns true if the player has died
	if(!p1alive && !p2alive){
		dialogue.SetText("Tie! Play again?");
		dialogue.SetPosition(App.GetWidth()/2-100, App.GetHeight()/2);
	}
	else if(p1alive){
		dialogue.SetText("Player 2 Wins! Play Again?");
		dialogue.SetPosition(App.GetWidth()/2-200, App.GetHeight()/2);
	}
	else{
		dialogue.SetText("Player 1 Wins! Play Again?");
		dialogue.SetPosition(App.GetWidth()/2-200, App.GetHeight()/2);
	}
	sf::Image choice_img[3];
	choice_img[0].LoadFromFile("imgs/select/fight.png");
	choice_img[1].LoadFromFile("imgs/select/retreat.png");
	choice_img[2].LoadFromFile("imgs/select/selector2.png");

	sf::Sprite choice[3];
	for(int i=0; i<3; i++){
		choice[i].SetImage(choice_img[i]);
		if(i<2)
			choice[i].SetPosition(400+300*i, 600);
	}

	int i=0;
	bool choosing=true;

	while(timer.GetElapsedTime()<10&&choosing){
		sf::Event Event;
		while(App.GetEvent(Event)){
			if(Event.Type==sf::Event::Closed)
				App.Close();
			if(Event.Type==sf::Event::KeyPressed && Event.Key.Code == sf::Key::Escape)
				App.Close();
		}

		if(App.GetInput().IsKeyDown(sf::Key::Left))
			if(i>0)
				i--;
		if(App.GetInput().IsKeyDown(sf::Key::Right))
			if(i<1)
				i++;
		if(App.GetInput().IsKeyDown(sf::Key::Return)){
			if(i==0)
				return true;
			else
				return false;
		}

		choice[2].SetPosition(choice[i].GetPosition().x-5, choice[i].GetPosition().y-5);

		std::ostringstream buff;
		buff << 10-(int)timer.GetElapsedTime();
		sf::String time(buff.str());
		time.SetPosition((float)App.GetWidth()/2, 100);

		App.Clear();
		App.Draw(time);
		App.Draw(dialogue);
		for(int i=0; i<3; i++)
			App.Draw(choice[i]);
		App.Display();
	}

	return false;

}

void handle_countdown(sf::RenderWindow &App){
	sf::Clock timer;
	timer.Reset();

	sf::String countdown;
	countdown.SetPosition((float)App.GetWidth()/2, 100);
	countdown.SetText("Fighters Ready!");
	while(timer.GetElapsedTime()<5){
		if(timer.GetElapsedTime()>4)
			countdown.SetText("FIGHT!");
		else if(timer.GetElapsedTime()>3)
			countdown.SetText("1...");
		else if(timer.GetElapsedTime()>2)
			countdown.SetText("2...");
		else if(timer.GetElapsedTime()>1)
			countdown.SetText("3...");

		App.Clear();
		App.Draw(countdown);
		App.Display();
	}

}

void welcome(sf::RenderWindow &App){
	sf::Image logo_img;
	logo_img.LoadFromFile("imgs/logo.png");
	sf::Sprite logo(logo_img, sf::Vector2f((float)App.GetWidth()/2-logo_img.GetWidth()/2, 
		(float)App.GetHeight()/2-logo_img.GetHeight()/2)); 
	sf::String hint("[Press any key to continue]");
	hint.SetPosition((float)App.GetWidth()/2-250, (float)App.GetHeight()-150);

	sf::Clock timer;
	bool lock = true;
	sf::Event Event;

	while(lock){
		float time=timer.GetElapsedTime();
		if(time>2)
			while(App.GetEvent(Event))
				if(Event.Type==sf::Event::KeyPressed)
					lock=false;
		App.Clear();
		App.Draw(logo);
		/*if(time>1)
		App.Draw(credits);*/
		if(time>3)
			App.Draw(hint);
		App.Display();
	}
}

void moveto(sf::Sprite &Sprite, sf::Vector2f dest, float elapsed){
	float angle = PI + atan2(Sprite.GetPosition().y - dest.y, Sprite.
		GetPosition().x - dest.x);
	Sprite.Move(30*cos(angle)*elapsed, 30*sin(angle)*elapsed);
}


player* character_select(sf::RenderWindow &App, std::string pos){
	//boring initializations
	sf::Image select_img[2];
	select_img[0].LoadFromFile("imgs/select/beth-select.png");
	select_img[1].LoadFromFile("imgs/select/dan-select.png");

	sf::Image selector_img;
	selector_img.LoadFromFile("imgs/select/selector.png");
	sf::Sprite selector(selector_img);

	std::vector <sf::Sprite> select(2);
	for(int i=0; i<2; i++){
		select[i].SetImage(select_img[i]);
		select[i].SetPosition(300+select[i].GetSize().x*i+100*i, 400);
	}

	selector.SetPosition(select[0].GetPosition().x-5, select[0].GetPosition().y-5);
	unsigned int i=0;

	bool choose=false;
	sf::Event Event;
	std::string prestring = "[" + pos + ", choose your fighter!]";
	sf::String string(prestring);
	string.SetPosition((float)App.GetWidth()/2 - 150, 100);

	//let them choose
	while(!choose){
		while(App.GetEvent(Event))
			if(Event.Type==sf::Event::Closed)
				App.Close();

		if(App.GetInput().IsKeyDown(sf::Key::Return))
			choose=true;
		App.Clear();
		App.Draw(string);
		if(App.GetInput().IsKeyDown(sf::Key::Right))
			if(i<select.size()-1)
				i++;

		if(App.GetInput().IsKeyDown(sf::Key::Left))
			if(i>0)
				i--;

		selector.SetPosition(select[i].GetPosition().x-5, select[i].GetPosition().y-5);

		for(int i=0; i<2; i++)
			App.Draw(select[i]);
		App.Draw(selector);
		App.Display();
	}

	sf::Vector2f dest = sf::Vector2f((float)App.GetWidth()/2, (float)App.GetHeight()/2);

	//display selection
	sf::Clock timer;
	while(timer.GetElapsedTime()<2){
		App.Clear();
		moveto(select[i], dest, App.GetFrameTime());
		App.Draw(select[i]);
		App.Display();
	}

	//return fighter selected
	if(i==0)
		return new FIGHTBETH(pos);
	if(i==1)
		return new FIGHTDAN(pos);

	return NULL;
}