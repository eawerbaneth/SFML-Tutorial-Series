import pygame
import math
import random

class enemy(object):
	"""the enemy class"""
	def __init__(self):
		self.images = [pygame.image.load("img/glue.png"), pygame.image.load("img/stapler.png")]
		self.image = self.images[random.randint(0, 1)]
		self.imgrect = self.image.get_rect()
		self.imgrect.move_ip(random.randint(0, 680-self.imgrect.width), -self.imgrect.height)
		self.yvel = 20
		self.health = 4
	def update(self, FrameRate):
		"""updates bullet"""
		FrameRate = FrameRate/100
		return self.move(FrameRate)
		
	def move(self, FrameRate):
		"""moves enemy along his trajectory"""
		self.imgrect.move_ip(0, self.yvel*FrameRate)
		if self.imgrect.top > 800:
			return False
		return True
		
	def draw(self, screen):
		screen.blit(self.image, self.imgrect)