#include "player.h"

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
	img.LoadFromFile("beth-idle-bnw.png");
	states.push_back(img);
	img.LoadFromFile("beth-block-bnw.png");
	states.push_back(img);
	img.LoadFromFile("beth-punch-bnw.png");
	states.push_back(img);
	img.LoadFromFile("beth-kick-bnw.png");
	states.push_back(img);
	img.LoadFromFile("beth-prespec-bnw.png");
	states.push_back(img);
	img.LoadFromFile("beth-spec-bnw.png");
	states.push_back(img);

	Sprite.SetImage(states[0]);

	//idle.LoadFromFile("beth-idle-bnw.tga");
	if(pos=="player1"){
		//Sprite.SetColor(sf::Color::Red);
		Sprite.SetPosition(100.f, 800.f-Sprite.GetSize().y);
		name.SetPosition(50, 50);
		health.set_pos(50.f);
	}
	else{
		//Sprite.SetColor(sf::Color::Blue);
		Sprite.FlipX(true);
		Sprite.SetPosition(1180.f-Sprite.GetSize().x, 800.f-Sprite.GetSize().y);
		name.SetPosition(800, 50);
		health.set_pos(800.f);
	}	
	state="NULL";

}

blast* FIGHTBETH::special(sf::Vector2f pos, bool dir){
	Sprite.SetColor(sf::Color::White);
	state="SPECIAL";
	return new blast(pos, dir);
}
