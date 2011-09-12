#include <SFML\Graphics.hpp>
#include <iostream>

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
		sf::Event Event;
		while(screen.GetEvent(Event)){
			if(Event.Type == sf::Event::Closed){
				screen.Close();
				return 0;
			}
			if(Event.Type==sf::Event::KeyPressed && Event.Key.Code == sf::Key::Escape){
				screen.Close();
				return 0;
			}
		}

		//mouse control
		old_mouse_coords = screen.ConvertCoords(mouse_coords.x, mouse_coords.y);
		mouse_coords = screen.ConvertCoords(screen.GetInput().GetMouseX(), 
					screen.GetInput().GetMouseY());
		if(screen.GetInput().IsMouseButtonDown(sf::Mouse::Left))
			View.Move(-1*(mouse_coords.x-old_mouse_coords.x), 
					-1*(mouse_coords.y-old_mouse_coords.y));
		
		//keyboard control
		if(screen.GetInput().IsKeyDown(sf::Key::Right))
			View.Move(10, 0);
		if(screen.GetInput().IsKeyDown(sf::Key::Left))
			View.Move(-10, 0);
		if(screen.GetInput().IsKeyDown(sf::Key::Up))
			View.Move(0, -10);
		if(screen.GetInput().IsKeyDown(sf::Key::Down))
			View.Move(0, 10);

		//maintain boundaries
		sf::Vector2f center = View.GetCenter();
		sf::Vector2f half_size = View.GetHalfSize();
		if(center.x-half_size.x<0)
			View.SetCenter(half_size.x, center.y);
		if(center.x+half_size.x>1000)
			View.SetCenter(1000-half_size.x, center.y);
		if(center.y-half_size.y<0)
			View.SetCenter(center.x, half_size.y);
		if(center.y+half_size.y>800)
			View.SetCenter(center.x, 800-half_size.y);


		//display
		screen.SetView(View);
		screen.Clear();
		screen.Draw(Background);
		screen.SetView(screen.GetDefaultView());
		screen.Display();
			
	}

	return 0;
}

