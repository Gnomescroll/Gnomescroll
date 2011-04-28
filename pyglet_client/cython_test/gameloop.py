from __future__ import division
from random import uniform

from pyglet import clock, font, image, window
from pyglet.gl import *

def draw(self):
    glLoadIdentity()
    glTranslatef(self.x, self.y, 0.0)
    glRotatef(self.rot, 0, 0, 1)
    glScalef(self.size, self.size, 1.0)
    glBegin(GL_TRIANGLES)
    glColor4f(1.0, 0.0, 0.0, 0.0)
    glVertex2f(0.0, 0.5)
    glColor4f(0.0, 0.0, 1.0, 1.0)
    glVertex2f(0.2, -0.5)
    glColor4f(0.0, 0.0, 1.0, 1.0)
    glVertex2f(-0.2, -0.5)
    glEnd()

from dat import CubeProperties#, CubeRenderCache, TerrainMap, MapChunkManager, MapChunk




from player import Player

from input import Mouse, Keyboard
from camera import Camera, Hud

from pyglet.window import key

#from World import World
#import pyximport; pyximport.install()
import cython
import pyximport; pyximport.install()
import world
#from world import World
#from world import World

class App(object):

    def __init__(self):

        self.world = world.World()
        self.win = window.Window(fullscreen=False, vsync=False)
        self.camera = Camera(self.win)
        self.camera = Camera(self.win)
        self.keyboard = Keyboard(self)
        self.player = Player() #for testing
        self.mouse = Mouse(self)
        self.hud = Hud(self.win)
        #clock.set_fps_limit(60)
        #setup events
        self.keyboard.key_handlers[key.ESCAPE] = self.win.close
        self.win.on_mouse_drag = self.mouse.on_mouse_drag
        self.win.on_key_press = self.keyboard.on_key_press

        print "App init finished"

    def mainLoop(self):
        clock.set_fps_limit(60)
        keyboard = key.KeyStateHandler()
        self.win.push_handlers(keyboard)
        while not self.win.has_exit:
            self.win.dispatch_events()
            self.keyboard.stateHandler(keyboard)

            self.world.tick()
            self.win.clear() #?

            self.camera.worldProjection()
            self.world.draw()
            self.camera.hudProjection()
            self.hud.draw()

            clock.tick()
            self.win.flip()

    def mainLoop2(self):
        self.world.test_chunk()
        self.world.add_player(self.player)
        #clock.set_fps_limit(60)
        keyboard = key.KeyStateHandler()
        self.win.push_handlers(keyboard)
        #self.win.push_handlers(pyglet.window.event.WindowEventLogger())
        while not self.win.has_exit:
            self.win.dispatch_events()
            self.keyboard.stateHandler(keyboard)

            self.world.tick()
            self.win.clear() #?

            self.camera.worldProjection()


            self.world.draw()
            self.camera.hudProjection()
            self.hud.draw()

            clock.tick()
            self.win.flip()
            #return

app = App()
#cProfile.run('app.mainLoop2()')
app.mainLoop2()

