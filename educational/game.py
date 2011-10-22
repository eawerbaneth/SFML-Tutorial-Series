import sys
import pygame
import math

class game():

	def __init__(self):
		"""The main running function"""
		#initialize window stuff
		self.windowx = 680
		self.windowy = 800
		pygame.init()
		self.clock = pygame.time.Clock()
		self.initialize_screen()
		self.frametime = 0.0
		self.no_exit
		
	def initialize_screen(self):
		"""Initialize the window"""
		self.screen = pygame.display.set_mode((self.windowx, self.windowy))
		pygame.display.set_caption("Proxy Name")
		pygame.mouse.set_visible(0)
		
	def draw(self):
		"""Draw everything on screen"""
		#Draw background
		#Draw player
		#Draw enemies
	
	def update(self):
		"""Update every frame"""
		
	def handle_collision(self, projectiles):
		"""handle all collision"""
		#do projectiles to enemies
		#do player collision
		
	def handle_events(self):
		"""Handle events (key presses, etc.)"""
		
	
	def run(self, already_run):
		"""Begin running the game"""
		if (not already_run):
			pass
		self.clock.tick()
		while self.no_exit:
			self.handle_events()
			self.update()
			self.draw()
			pygame.display.flip()
			self.frametime = float(self.clock.tick(60))
	
	def exit_game(self):
		"""Exit the game"""
		pygame.quit()
		sys.exit()
