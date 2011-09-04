#include <iostream>
#include <sstream>
#include "player.h"
#include <fstream>

int main(){

	//for now I'm going to keep it in development mode for debugging purposes
	std::cout << "Development Mode:\n";

	//initialize the game window
	sf::RenderWindow App(sf::VideoMode(1280, 800), "SFML FIGHT!");
	player* player_1=new FIGHTBETH("player1");
	player* player_2=new FIGHTBETH("player2");

	//std::string test=player_1->get_name().GetText();


	bool p1lock=false, p2lock=false;
	bool p1_block=false, p2_block=false;
	bool p1_special=false, p2_special=false;
	sf::Clock p1Clock, p2Clock;
	sf::Clock timer;
	bool game_over=false;
	special_attack* p1spec;
	special_attack* p2spec;

	sf::Image Image;
	Image.LoadFromFile("beth-idle-bnw.png");
	sf::Sprite Sprite(Image);

	/*
	//countdown!
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
	*/

	//GAME LOOP
	timer.Reset();
	while(App.IsOpened()&&timer.GetElapsedTime()<30&&game_over==false){

		//EVENT HANDLING
		sf::Event Event;
		while(App.GetEvent(Event)){
			if(Event.Type==sf::Event::Closed)
				App.Close();
		}

		//MAINTAIN CONSISTENCY
		float ElapsedTime = App.GetFrameTime();

		//PLAYER ONE CONTROLS
		if(p1lock==false){
			//movement
			if(App.GetInput().IsKeyDown(sf::Key::A))
				player_1->adjust(-150*ElapsedTime, 0);
			if(App.GetInput().IsKeyDown(sf::Key::D))
				player_1->adjust(150*ElapsedTime, 0);
			//punch
			if(App.GetInput().IsKeyDown(sf::Key::G)){
				if(player_1->punch(player_2->get_pos(), player_2->get_state()))
					game_over=player_2->take_damage(10);
				p1lock=true;
				p1Clock.Reset();
			}
			//kick
			else if(App.GetInput().IsKeyDown(sf::Key::V)){
				if(player_1->kick(player_2->get_pos(), player_2->get_state()))
					game_over=player_2->take_damage(25);
				p1lock=true;
				p1Clock.Reset();
			}
			//special
			else if(App.GetInput().IsKeyDown(sf::Key::B)){
				p1_special=true;
				//if we are to the left of the enemy
				if(player_1->get_pos().x < player_2->get_pos().x)
					p1spec=player_1->special(player_1->get_pos(), true);
				else
					p1spec=player_1->special(player_1->get_pos(), false);
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
			if(player_1->get_state()=="KICK"&&p1Clock.GetElapsedTime()>=0.5){
				player_1->release_state(player_2->get_pos());
				p1lock=false;
			}
			else if(player_1->get_state()=="PUNCH"&&p1Clock.GetElapsedTime()>=0.25){
				player_1->release_state(player_2->get_pos());
				p1lock=false;
			}
			else if(player_1->get_state()=="SPECIAL"&&p1Clock.GetElapsedTime()>=1){
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
				player_2->adjust(-150*ElapsedTime, 0);
			if(App.GetInput().IsKeyDown(sf::Key::Right))
				player_2->adjust(150*ElapsedTime, 0);
			//punch
			if(App.GetInput().IsKeyDown(sf::Key::SemiColon)){
				if(player_2->punch(player_1->get_pos(), player_1->get_state()))
					game_over=player_1->take_damage(10);
				p2lock=true;
				p2Clock.Reset();
			}
			//kick
			else if(App.GetInput().IsKeyDown(sf::Key::Period)){
				if(player_2->kick(player_1->get_pos(), player_1->get_state()))
					game_over=player_1->take_damage(25);
				p2lock=true;
				p2Clock.Reset();
			}
			//special
			else if(App.GetInput().IsKeyDown(sf::Key::Slash)){
				p2_special=true;
				if(player_2->get_pos().x < player_1->get_pos().x)
					p2spec=player_2->special(player_2->get_pos(), true);
				else
					p2spec=player_2->special(player_2->get_pos(), false);
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
			if(player_2->get_state()=="KICK"&&p2Clock.GetElapsedTime()>=0.5){
				player_2->release_state(player_1->get_pos());
				p2lock=false;
			}
			else if(player_2->get_state()=="PUNCH"&&p2Clock.GetElapsedTime()>=0.25){
				player_2->release_state(player_1->get_pos());
				p2lock=false;
			}
			else if(player_2->get_state()=="SPECIAL"&&p2Clock.GetElapsedTime()>=1){
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
		buff << 30-timer.GetElapsedTime();
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
			if(p1spec->collides(player_2->get_pos(), player_2->get_Sprite().GetSize(), 
				player_2->get_state())){
					game_over=player_2->take_damage(40);
					p1_special=false;
			}
			App.Draw(p1spec->get_image(ElapsedTime, p1_special));
		}
		if(p2_special){
			if(p2spec->collides(player_1->get_pos(), player_1->get_Sprite().GetSize(), 
				player_1->get_state())){
					game_over=player_1->take_damage(40);
					p2_special=false;
			}
			App.Draw(p2spec->get_image(ElapsedTime, p2_special));
		}
		//drawing characters
		App.Draw(player_1->get_Sprite());
		App.Draw(player_2->get_Sprite());
		App.Draw(Sprite);
		App.Display();

	}

	return 0;
}