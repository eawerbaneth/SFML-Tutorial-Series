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
	def __init__(self, winx, winy, money):
		self.conversation = 0
		self.active = True
		self.debt = 0
		self.cursorpos = 0
		self.interest = .40
		self.background = pygame.image.load("img/sharktext.png")#.convert()
		self.backrect = self.background.get_rect()
		self.winx = winx
		self.winy = winy
		self.font32 = pygame.font.Font("img/SVBasicManual.ttf", 48)
		self.cursor = pygame.image.load("img/cursor.png")#.convert()
		self.cursorrect = self.cursor.get_rect()
		
		self.takebtn = pygame.image.load("img/loan.png")
		self.takerect = self.takebtn.get_rect()
		self.takerect.move_ip(50, 100)
		
		self.backbtn = pygame.image.load("img/back.png")
		self.btnrect = self.backbtn.get_rect()
		self.btnrect.move_ip(150, 300)
		
		self.paper = pygame.image.load("img/paper.png")
		self.paperrect = self.paper.get_rect()
		self.money = money
	
	def borrow(self):
		self.debt -= 10000
		self.money += 10000
	
	def activate(self, screen, money):
		self.debt += self.debt*self.interest
		self.money = money
	
		self.active = True
		while self.active:
			#print "updating shark"
			self.handle_events()
			self.draw(screen)
			pygame.display.flip()
			
		return -self.debt
	
	def draw(self, screen):
		#print "drawing shark"
		screen.blit(self.paper, self.paperrect)
		screen.blit(self.background, pygame.Rect(0, 0, self.winx, self.winy))
		screen.blit(self.takebtn, self.takerect)
		screen.blit(self.backbtn, self.btnrect)
		screen.blit(self.cursor, self.cursorrect)
		
		#draw amount of money
		txtsurf2 = self.font32.render("Money: " + str(self.money), 1, (0,0,0))
		txtrect2 = txtsurf2.get_rect()
		txtrect2.center = (300, 600)
		screen.blit(txtsurf2, txtrect2)
		
		#draw amount of debt
		txtsurf2 = self.font32.render("Debt: " + str(-self.debt), 1, (0,0,0))
		txtrect2 = txtsurf2.get_rect()
		txtrect2.center = (300, 700)
		screen.blit(txtsurf2, txtrect2)
	
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
			
		self.cursorrect.move_ip(50 + self.cursorpos*100 - self.cursorrect.left, 100 + self.cursorpos*200 - self.cursorrect.top)	
		
			
class Shop():
	def __init__(self, winx, winy):
		"""initialize the shop!"""
		self.money = 2500
		self.inventory = []
		self.do_inven()
		self.active = False
		self.windowx = winx
		self.windowy = winy
		self.background = pygame.image.load("img/storewithpigtext.png")#.convert()
		self.cursor_pos = [0, 0]
		self.cursor = pygame.image.load("img/cursor.png")#.convert()
		self.cursorrect = self.cursor.get_rect()
		self.my_inventory = [False, False, False, False]
		self.sharkconvo = Shark(winx, winy, self.money)
		self.sharkimg = pygame.image.load("img/sharkalone.png")#.convert()
		self.sharkrect = self.sharkimg.get_rect()
		self.sharkrect.move_ip(50, 350)
		self.backimg = pygame.image.load("img/back.png")#.convert()
		self.backrect = self.backimg.get_rect()
		self.backrect.move_ip(200, 350)
		self.paper = pygame.image.load("img/paper.png")
		self.paperrect = self.paper.get_rect()
		self.font32 = pygame.font.Font("img/SVBasicManual.ttf", 48)
        
	def do_inven(self):
		#name, price, description, x, y, path
		self.inventory = [[item("Chip", 10000, "descript", 50, 50, "img/chipstore.png"), item("Lightning", 10000, "descript", 200, 50, "img/lightningstore.png")],[item("Eraser", 10000, "descript", 50, 200, "img/eraserstore.png"), item("Cube",10000, "descript", 200, 200, "img/cubestore.png")]]
	
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
		self.my_inventory[self.cursor_pos[0]+2*self.cursor_pos[1]] = True
		self.my_inventory.append(self.inventory[self.cursor_pos[0]][self.cursor_pos[1]])
		self.money -= self.inventory[self.cursor_pos[0]][self.cursor_pos[1]].buy()
		
	def draw(self, screen):
		#background
		screen.blit(self.paper, self.paperrect)
		screen.blit(self.background, pygame.Rect(0, 0, self.windowx, self.windowy))
		#draw items
		for row in self.inventory:
			for item in row:
				item.draw(screen)
		
		#draw amount of money
		txtsurf2 = self.font32.render("Money: " + str(self.money), 1, (0,0,0))
		txtrect2 = txtsurf2.get_rect()
		txtrect2.center = (500, 50)		
		screen.blit(txtsurf2, txtrect2)
		
		#draw cursor
		screen.blit(self.cursor, self.cursorrect)
			
		#draw shark image
		screen.blit(self.sharkimg, self.sharkrect)
		
		#draw exit image
		screen.blit(self.backimg, self.backrect)
		
		if(self.cursor_pos[0] < 2):
			txtsurf = self.font32.render(self.inventory[self.cursor_pos[0]][self.cursor_pos[1]].description, 1, (0,0,0))
			txtrect = txtsurf.get_rect()
			txtrect.center = (300, 600)
		
			screen.blit(txtsurf, txtrect)
		
			
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
						if self.money >= self.inventory[self.cursor_pos[0]][self.cursor_pos[1]].price and not self.inventory[self.cursor_pos[0]][self.cursor_pos[1]].sold:
							self.buy()
					elif self.cursor_pos[1]==0:
						self.money += self.sharkconvo.activate(screen, self.money)
					else:
						self.active = False
			
		self.cursorrect.move_ip(50 + self.cursor_pos[1]*150 - self.cursorrect.left, 50+self.cursor_pos[0]*150 - self.cursorrect.top)
		#print "moving cursor to ", self.cursor_pos[0], self.cursor_pos[1]
		#if self.cursor_pos[1] == 2 and self.cursor_pos[0] == 0:
		#	self.cursorrect.move_ip(50, 600)
		#elif self.cursor_pos[1] == 2 and self.cursor_pos[0] == 1:
		#	self.cursorrect.move_ip(200, 600)
		