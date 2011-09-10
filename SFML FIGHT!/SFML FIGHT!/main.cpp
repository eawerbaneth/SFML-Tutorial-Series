#include <sstream>
#include "player.h"
#include <fstream>
#include <time.h>

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
	/*sf::String credits("Programmed by Beth Werbaneth");
	credits.SetPosition((float)App.GetWidth()/2-250, (float)App.GetHeight()/2+150);*/
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