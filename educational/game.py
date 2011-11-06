import sys
import pygame
import math
from key_bindings import key_bindings
from player import player
from bullet import bullet
from shop import Shop

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
        self.no_exit = True
        self.distance = 0
        self.lives = 5
        self.immortal = 2000
        self.last_death = -2000
        self.player_killed = False
        self.in_level = False
        self.money = 0
        
        self.player = player(self)
		
        self.myshop = Shop(self.windowx, self.windowy)
        self.shopactive = True
		
		#images 'n' such
        self.background = pygame.image.load("img/background.png")#.convert()
        self.deaddraw = True
        self.deaddrawnum = 0 #makes player flicker when respawning
        self.levelactive = False
        self.oldflag = self.levelactive
        
        self.run(False)
		
		
    def initialize_screen(self):
        """Initialize the window"""
        self.screen = pygame.display.set_mode((self.windowx, self.windowy))
        pygame.display.set_caption("Proxy Name")
        pygame.mouse.set_visible(0)
		
    def draw(self):
        """Draw everything on screen"""
		#Draw background
        backrect = self.background.get_rect()
        ydisp = (self.distance/2)%backrect.height
        ydisp2 = (self.distance/4)%backrect.height
        self.screen.blit(self.background, pygame.Rect(0, ydisp, self.windowx, self.windowy))
        self.screen.blit(self.background, pygame.Rect(100, ydisp - backrect.height,self.windowx, self.windowy))
		#Draw player
        if self.distance > self.last_death + self.immortal_time or self.deaddraw:
            self.player.draw(self.screen)
        self.deaddrawnum += 1
        if self.deaddrawnum > 10:
            self.deaddrawnum = 0
            seld.deaddraw = not(self.deaddraw)
		#Draw enemies
        
	
    def update(self):
        """Update every frame"""
		#update player
        self.distance += self.frametime
        projectiles = []
        if self.lives > 0:
            projectiles = self.player.update(self.frametime)
        else:
            txtsurf = self.font32.render("GAME OVER", 1, (0,0,0))
            txtrect = txtsurf.get_rect()
            txtrect.center = (300, self.windowy/2)
            self.screen.blit(txtsurf, txtrect)
            pygame.display.flip()
            pygame.time.wait(2000)
            self.exit_game()
        if self.player_killed == True:
            if self.distance > self.last_death + self.immortal_time:
                self.last_death = self.distance
                self.lives -= 1
                self.player_killed = False
            else:
                self.player_killed = False
        #update enemies
        
        #handle collision
        self.handle_collision(projectiles)
		
		
    def handle_collision(self, projectiles):
        """handle all collision"""
        #do projectiles to enemies
        #do player collision
        
    def handle_events(self):
        """Handle events (key presses, etc.)"""
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.exit_game()
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    self.exit_game()
                if self.in_level == True:
					#movement
                    if event.key in self.key_bindings.up:
                        self.player.moving[0] = True
                    if event.key in self.key_bindings.down:
                        self.player.moving[1] = True
                    if event.key in self.key_bindings.left:
                        self.player.moving[2] = True
                    if event.key in self.key_bindings.right:
                        self.player.moving[3] = True
                    #abilities
                    if event.key in self.key_bindings.shoot:
                        self.player.shoot = True
            if event.type == pygame.KEYUP:
                #cancelling movement
                if event.key in self.key_bindings.up:
                    self.player.moving[0] = False
                if event.key in self.key_bindings.down:
                    self.player.moving[1] = False
                if event.key in self.key_bindings.left:
                    self.player.moving[2] = False
                if event.key in self.key_bindings.right:
                    self.player.moving[3] = False
                #cancelling abilities
                if event.key in self.key_bindings.shoot:
                    self.player.shoot = False
		
	
    def run(self, already_run):
        """Begin running the game"""
        if (not already_run):
            pass
        self.clock.tick()
        while self.no_exit:
            print "updating..."
            if (self.levelactive):
                self.handle_events()
                self.update()
                self.draw()
                pygame.display.flip()
            else:
                self.myshop.activate(self.screen, self.money)
                self.money = 0
                self.levelactive = True

            self.frametime = float(self.clock.tick(60))
    
    def exit_game(self):
        """Exit the game"""
        pygame.quit()
        sys.exit()
