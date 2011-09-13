#include <SFML\Graphics.hpp>
#include <iostream>

//return true if exit command received
bool handle_events(sf::RenderWindow &screen, sf::View &View);
void keyboard_input(sf::RenderWindow &screen, sf::View &View);
void police_boundaries(sf::View &View);

int main(){
	//initalize the game window
	sf::RenderWindow screen(sf::VideoMode(800, 600), "God of Fire");
	screen.SetFramerateLimit(30);

	//fun initializations
	sf::Vector2f old_mouse_coords;
	sf::Vector2f mouse_coords;
	sf::Image BackgroundImage;
	BackgroundImage.LoadFromFile("background.png");
	sf::Sprite Background(BackgroundImage);

	sf::View View=screen.GetDefaultView();
	while(screen.IsOpened()){

		if(handle_events(screen, View))
			screen.Close();

		//mouse control
		old_mouse_coords = screen.ConvertCoords((unsigned)mouse_coords.x, (unsigned)mouse_coords.y);
		mouse_coords = screen.ConvertCoords(screen.GetInput().GetMouseX(), 
			screen.GetInput().GetMouseY());
		if(screen.GetInput().IsMouseButtonDown(sf::Mouse::Left))
			View.Move(-1*(mouse_coords.x-old_mouse_coords.x), 
			-1*(mouse_coords.y-old_mouse_coords.y));

		//keyboard control
		keyboard_input(screen, View);

		//maintain boundaries
		police_boundaries(View);

		//display
		screen.SetView(View);
		screen.Clear();
		screen.Draw(Background);
		screen.SetView(screen.GetDefaultView());
		screen.Display();

	}

	return 0;
}

void police_boundaries(sf::View &View){
		sf::Vector2f center = View.GetCenter();
		sf::Vector2f half_size = View.GetHalfSize();
		if(center.x-half_size.x<0)
			View.SetCenter(half_size.x, center.y);
		if(center.x+half_size.x>1000)
			View.SetCenter(1000-half_size.x, center.y);
		center = View.GetCenter();
		if(center.y-half_size.y<0)
			View.SetCenter(center.x, half_size.y);
		if(center.y+half_size.y>800)
			View.SetCenter(center.x, 800-half_size.y);

}

void keyboard_input(sf::RenderWindow &screen, sf::View &View){
	if(screen.GetInput().IsKeyDown(sf::Key::Right))
		View.Move(20, 0);
	if(screen.GetInput().IsKeyDown(sf::Key::Left))
		View.Move(-20, 0);
	if(screen.GetInput().IsKeyDown(sf::Key::Up))
		View.Move(0, -20);
	if(screen.GetInput().IsKeyDown(sf::Key::Down))
		View.Move(0, 20);
	if(screen.GetInput().IsKeyDown(sf::Key::Add) 
		|| screen.GetInput().IsKeyDown(sf::Key::Equal))
		View.Zoom(1.1f);
	if(screen.GetInput().IsKeyDown(sf::Key::Subtract) 
		|| screen.GetInput().IsKeyDown(sf::Key::Dash))
		if(View.GetRect().GetWidth()/0.9 < screen.GetWidth())
			View.Zoom(0.9f);

}

bool handle_events(sf::RenderWindow &screen, sf::View &View){
	sf::Event Event;
	while(screen.GetEvent(Event)){
		//handles exits
		if(Event.Type == sf::Event::Closed)
			return true;
		if(Event.Type==sf::Event::KeyPressed && Event.Key.Code == sf::Key::Escape)
			return true;
		//handles scrolling
		if(Event.Type==sf::Event::MouseWheelMoved){
			float mod = 1;
			if(Event.MouseWheel.Delta>0)
				mod = 1+((float)Event.MouseWheel.Delta)*.1f;
			else
				mod =(float)(90+(float)Event.MouseWheel.Delta)/100.0f;
			if(View.GetRect().GetWidth()/mod < screen.GetWidth())
				View.Zoom(mod);
		}
	}
	return false;

}