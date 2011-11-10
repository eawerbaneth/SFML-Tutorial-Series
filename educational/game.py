import sys
import pygame
import math
from key_bindings import key_bindings
from player import player
from bullet import bullet
from shop import Shop
from enemy import enemy

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
        self.lives = 3
        self.immortal = 2000
        self.last_death = -2000
        self.player_killed = False
        self.in_level = False
        self.money = 0
        self.font32 = pygame.font.Font("img/SVBasicManual.ttf", 20)
        self.addons = [pygame.image.load("img/chip.png"), pygame.image.load("img/lightning.png"), pygame.image.load("img/eraser.png"), pygame.image.load("img/cube.png")]
        
        self.player = player(self)
        self.key_bindings = key_bindings()
        self.enemies = []
		
        self.myshop = Shop(self.windowx, self.windowy)
        self.shopactive = True
		
		#images 'n' such
        self.background = pygame.image.load("img/floor2.png")#.convert()
        self.deaddraw = True
        self.deaddrawnum = 0 #makes player flicker when respawning
        self.levelactive = False
        self.oldflag = self.levelactive
        self.frame = 0
        
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
        self.screen.blit(self.background, pygame.Rect(0, ydisp - backrect.height,self.windowx, self.windowy))
		#Draw player
        if self.distance > self.last_death + self.immortal or self.deaddraw:
            self.player.draw(self.screen)
            for i, flag in enumerate(self.myshop.my_inventory):
                if flag == True:
                    self.screen.blit(self.addons[i], self.player.rect)
        self.deaddrawnum += 1
        if self.deaddrawnum > 10:
            self.deaddrawnum = 0
            self.deaddraw = not(self.deaddraw)
		#Draw enemies
        for anenome in self.enemies:
            anenome.draw(self.screen)
        #Draw HUD
        txtsurf = self.font32.render("Money: "+ str(self.myshop.money + self.money), 1, (255,255,255))
        txtrect = txtsurf.get_rect()
        txtrect.center = (100, 100)
        self.screen.blit(txtsurf, txtrect)
        
	
    def update(self):
        """Update every frame"""
		#update player
        self.distance += self.frametime
        self.player.update(self.frametime)
        projectiles = []
        if self.lives > 0:
            projectiles = self.player.update(self.frametime)
        else:
            txtsurf = self.font32.render("GAME OVER", 1, (255,255,255))
            txtrect = txtsurf.get_rect()
            txtrect.center = (300, self.windowy/2)
            self.screen.blit(txtsurf, txtrect)
            pygame.display.flip()
            pygame.time.wait(2000)
            self.exit_game()
        if self.player_killed == True:
            if self.distance > self.last_death + self.immortal:
                self.last_death = self.distance
                self.lives -= 1
                self.player_killed = False
            else:
                self.player_killed = False
        #update enemies
        for i, anenome in enumerate(self.enemies):
            if not anenome.update(self.frametime):
                self.enemies.pop(i)
        #handle collision
        self.handle_collision(projectiles)
		
    def handle_collision(self, projectiles):
        """handle all collision"""
        #do projectiles to enemies
        for projectile in projectiles:
            for i, anenome in enumerate(self.enemies):
                if projectile.rect.colliderect(anenome.imgrect):
                    anenome.health -= 1
                    if anenome.health == 0:
                        self.enemies.pop(i)
                    projectile.hit = True
                    self.money += 5
        for i, anenome in enumerate(self.enemies):
            if self.player.rect.colliderect(anenome.imgrect):
                self.enemies.pop(i)
                self.lives -= 1
                        
                
        #do player collision
        
    def handle_events(self):
        """Handle events (key presses, etc.)"""
        #print "handling events..."
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.exit_game()
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    self.exit_game()
					#movement
                if event.key in self.key_bindings.up:
                    #print "up"
                    self.player.moving[0] = True
                if event.key in self.key_bindings.down:
                    #print "down"
                    self.player.moving[1] = True
                if event.key in self.key_bindings.left:
                    #print "left"
                    self.player.moving[2] = True
                if event.key in self.key_bindings.right:
                    #print "right"
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
            #print "updating..."
            self.frame += 1
            probability = 50+(self.myshop.sharkconvo.debt/1000)
            if probability < 1:
                probability = 1
            print "probability is ", probability
            if self.frame%probability==0:
                self.enemies.append(enemy())
            if self.frame >= 900:
                self.levelactive = False
                self.frame = 0
            if (self.levelactive):
                self.handle_events()
                self.update()
                self.draw()
                pygame.display.flip()
            else:
                self.myshop.activate(self.screen, self.money + 2500)
                self.money = 0
                self.levelactive = True

            self.frametime = float(self.clock.tick(60))
    
    def exit_game(self):
        """Exit the game"""
        pygame.quit()
        sys.exit()
