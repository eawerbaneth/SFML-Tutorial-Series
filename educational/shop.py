import pygame
import math

class item():
	def __init__(self, name, price, description, x, y, path):
		"""initializes each item"""
		self.price = price
		self.name = name
		self.description = description
		self.sold = False
		self.image = pygame.image.load(path)#.convert()
		self.imgrect = self.image.get_rect()
		self.imgrect.move_ip(x, y)
		self.soldout = pygame.image.load("img/soldout.png")#.convert()
		self.soldrect = self.soldout.get_rect()
		self.soldrect.move_ip(x,y)
		
	def buy(self):
		self.sold = True
		return self.price
		
	def draw(self, screen):
		"""draw the items"""
		if self.sold == False:
			screen.blit(self.image, self.imgrect)
		else:
			screen.blit(self.soldout, self.soldrect)

class Shark():
	def __init__(self, winx, winy):
		self.conversation = 0
		self.active = True
		self.debt = 0
		self.cursorpos = 0
		self.interest = .40
		self.background = pygame.image.load("img/shark.png")#.convert()
		self.backrect = self.background.get_rect()
		self.winx = winx
		self.winy = winy
		
		self.cursor = pygame.image.load("img/cursor.png")#.convert()
		self.cursorrect = self.cursor.get_rect()
		
		self.takebtn = pygame.image.load("img/take_load.png")
		self.takerect = self.takebtn.get_rect()
		self.takerect.move_ip(50, 100)
		
		self.backbtn = pygame.image.load("img/back.png")
		self.btnrect = self.backbtn.get_rect()
		self.btnrect.move_ip(300, 100)
	
	def borrow(self):
		self.debt -= 10000
	
	def activate(self, screen):
		self.debt += self.debt*self.interest
	
		self.active = True
		while self.active:
			print "updating shark"
			self.handle_events()
			self.draw(screen)
			pygame.display.flip()
			
		return -self.debt
	
	def draw(self, screen):
		print "drawing shark"
		screen.blit(self.background, pygame.Rect(0, 0, self.winx, self.winy))
		screen.blit(self.takebtn, self.takerect)
		screen.blit(self.backbtn, self.btnrect)
		screen.blit(self.cursor, self.cursorrect)
	
	def handle_events(self):
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				pygame.quit()
				sys.exit()
			elif event.type == pygame.KEYDOWN:
				if event.key == pygame.K_ESCAPE:
					self.active = False
				if event.key == pygame.K_LEFT:
					if self.cursorpos == 1:
						self.cursorpos = 0
				if event.key == pygame.K_RIGHT:
					if self.cursorpos == 0:
						self.cursorpos = 1
				if event.key == pygame.K_RETURN:
					if self.cursorpos == 0:
						self.borrow()
					else:
						self.active = False
			
		self.cursorrect.move_ip(50 + self.cursorpos*250 - self.cursorrect.left, 0)	
		
			
class Shop():
	def __init__(self, winx, winy):
		"""initialize the shop!"""
		self.money = 5000
		self.inventory = []
		self.do_inven()
		self.active = False
		self.windowx = winx
		self.windowy = winy
		self.background = pygame.image.load("img/storewithpig.png")#.convert()
		self.cursor_pos = [0, 0]
		self.cursor = pygame.image.load("img/cursor.png")#.convert()
		self.cursorrect = self.cursor.get_rect()
		self.my_inventory = []
		self.sharkconvo = Shark(winx, winy)
		self.sharkimg = pygame.image.load("img/sharkalone.png")#.convert()
		self.sharkrect = self.sharkimg.get_rect()
		self.sharkrect.move_ip(50, 400)
		self.backimg = pygame.image.load("img/back.png")#.convert()
		self.backrect = self.backimg.get_rect()
		self.backrect.move_ip(200, 400)
	
	def do_inven(self):
		#name, price, description, x, y, path
		self.inventory = [[item("Chip", 10000, "descript", 50, 50, "img/chipstore.png"), item("Cube",10000, "descript", 200, 50, "img/cubestore.png")],[item("Eraser", 10000, "descript", 50, 200, "img/eraserstore.png"), item("Lightning", 10000, "descript", 200, 200, "img/lightningstore.png")]]
	
	def activate(self, screen, mo_money):
		self.active = True
		self.money += mo_money
		while self.active:
			#print "shop updating..."
			self.handle_events(screen)
			self.draw(screen)
			pygame.display.flip()
		
	def exit_shop(self):
		self.active = False
		
	def buy(self):
		self.my_inventory.append(self.inventory[self.cursor_pos[0]][self.cursor_pos[1]])
		self.money -= self.inventory[self.cursor_pos[0]][self.cursor_pos[1]].buy()
		
	def draw(self, screen):
		#background
		screen.blit(self.background, pygame.Rect(0, 0, self.windowx, self.windowy))
		#draw items
		for row in self.inventory:
			for item in row:
				item.draw(screen)
		#draw cursor
		screen.blit(self.cursor, self.cursorrect)
			
		#draw shark image
		screen.blit(self.sharkimg, self.sharkrect)
		
		#draw exit image
		screen.blit(self.backimg, self.backrect)
		
		
			
	def handle_events(self, screen):
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				pygame.quit()
				sys.exit()
			elif event.type == pygame.KEYDOWN:
				if event.key == pygame.K_ESCAPE:
					self.active = False
				if event.key == pygame.K_UP:
					if self.cursor_pos[0] > 0:
						self.cursor_pos[0] -= 1
				if event.key == pygame.K_DOWN:
					if self.cursor_pos[0] < 2:
						self.cursor_pos[0]+=1
				if event.key == pygame.K_LEFT:
					if self.cursor_pos[1] > 0:
						self.cursor_pos[1] -= 1
				if event.key == pygame.K_RIGHT:
					if self.cursor_pos[1] < 1:
						self.cursor_pos[1] += 1
				if event.key == pygame.K_RETURN:
					if self.cursor_pos[0] != 2:
						self.buy()
					elif self.cursor_pos[1]==0:
						self.money += self.sharkconvo.activate(screen)
					else:
						self.active = False
			
		self.cursorrect.move_ip(50 + self.cursor_pos[1]*150 - self.cursorrect.left, 50+self.cursor_pos[0]*150 - self.cursorrect.top)	
		print "moving cursor to ", self.cursor_pos[0], self.cursor_pos[1]
		#if self.cursor_pos[1] == 2 and self.cursor_pos[0] == 0:
		#	self.cursorrect.move_ip(50, 600)
		#elif self.cursor_pos[1] == 2 and self.cursor_pos[0] == 1:
		#	self.cursorrect.move_ip(200, 600)
		