import pygame
import math

class player(object):
	"""the player's fighter ship"""
	def __init__(self, game):
		self.energy = 0.0
		self.image = pygame.image.load("imgs/fighter_proxy.png")
		self.rect = self.image.get_rect()
		self.rect.width = 60
		#move koi to the middle of the screen
		self.rect.move_ip(game.windowx/2-self.rect.width/2, game.windowy/2-self.rect.height/2)
		self.xvel = 35
		self.yvel = 35
		#CONTROL LOCKS
		#movement: up, down, left, right
		self.moving = [False, False, False, False]
		#shooting
		self.shoot = False
		self.shoot_cooldown = 0.75
		self.projectiles = []
		#animation utilities
		self.game = game
		self.frames = 0
		
		self.death = False
		
	def update(self, FrameRate):
		"""handles input"""
		FrameRate = FrameRate/100
		self.frames += 1
		
	
	#MOVEMENT
	def move(self, FrameRate):
		"""move the fighter ship"""
		updown_lock = False
		leftright_lock = False
		# negate any contradictory movements
		if self.moving[0] and self.moving[1]:
			updown_lock = True
		elif self.moving[2] and self.moving[3]:
			leftright_lock = True
		
		if not updown_lock:
			#up
			if self.moving[0]:
				future = self.rect.move(0, -self.yvel*FrameRate)
				if(future.top < 400):
					self.rect.top = 400
				else:
					self.rect = future
			#down
			elif self.moving[1]:
				future = self.rect.move(0, self.yvel*FrameRate)
				if(future.bottom>game.windowy):
					self.rect.bottom = game.windowy
				else:
					self.rect = future
		if not leftright_lock:
			#left
			if self.moving[2]:
				future = self.rect.move(-self.xvel*FrameRate, 0)
				if future.left < 0:
					self.rect.left = 0
				else:
					self.rect = future
			#right
			elif self.moving[3]:
				future = self.rect.move(self.xvel*FrameRate, 0)
				if future.right > game.windowx:
					self.rect.right = game.windowx
				else:
					self.rect = future
	
	def handle_shoot(self, FrameRate):
		"""handle shooting"""
		self.shoot_cooldown -= FrameRate
		if self.shoot_cooldown < 0.0:
			self.shoot_cooldown = 0.0
			
		if self.shoot == True and self.shoot_cooldown == 0.0:
			new_bullet = bullet(self.rect.left + 16, self.rect.top, math.pi/2)
			self.projectiles.append(new_bullet)
			self.shoot_cooldown = 2.0
		for i, projectile in enumerate(self.projectiles):
			if not projectiles.update(FrameRate):
				self.projectiles.pop(i)
	
	def draw(self, screen):
		"""draws fighter"""
		screen.blit(self.image, self.rect, Pygame.Rect(0, 0, self.rect.width, self.rect.height))
		
		for projectile in self.projectiles:
			pass
			#projectile.draw(screen)
