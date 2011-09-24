import sys
import os
sys.path.insert(0, './ext/')

import _Noise as N

from pyglet import window
from pyglet import clock
from pyglet import font
import pyglet

class Sprite(object):
    def __init__(self, image_file=None, image_data=None):

        #init standard variables
        '''
        self.image_file = image_file
        if (image_data is None):
            self.image = helper.load_image(image_file)
        else:
            self.image = image_data
        '''
        #self.image = pyglet.image.BufferImage(30,30,64,64)
        #self.image = Class pyglet.image.ImageData(64,64,
        self.texture = pyglet.image.Texture.create(64,64, pyglet.gl.GL_RGBA)
        self.x = 150
        self.y = 150

        '''
        d = self.texture.get_image_data()
        d1 = d.get_data('RGBA',d.width*4)

        z = 0.5
        for i in range(0,4096):
            x = i % 64
            y = i / 64
            l = N._p3(x,y,z)
            print(str(l))
            l = int(l)
            d1[4*i+0] = l
            d1[4*i+1] = l
            d1[4*i+2] = l
            d1[4*i+3] = 0

        self.image = pyglet.image.ImageData(64,64,'RGBA',d1)
        '''
        self.z = 0
        self.update()

    def draw(self):
        #self.image.blit(self.x, self.y)
        #self.texture.blit(self.x, self.y)
        self.image.blit(self.x,self.y)

    def update(self):
        d = self.texture.get_image_data()
        d1 = d.get_data('RGBA',d.width*4)

        self.z += 0.01
        for i in range(0,4096):
            x = i % 64
            y = i / 64
            l = 1+N._p3(x/64.0,y/64.0,self.z)
            if l < 0:
                l=0
            if l > 1:
                l=1
            #print(str(l))
            l = int(255*l)
            d1[4*i+0] = l
            d1[4*i+1] = l
            d1[4*i+2] = l
            d1[4*i+3] = 0

        self.image = pyglet.image.ImageData(64,64,'RGBA',d1)

import random

class SpaceGameWindow(window.Window):

    def __init__(self, *args, **kwargs):

        #Let all of the standard stuff pass through
        window.Window.__init__(self, *args, **kwargs)

    def main_loop(self):

        #Create a font for our FPS clock
        ft = font.load('Arial', 28)
        #The pyglet.font.Text object to display the FPS
        fps_text = font.Text(ft, y=10)
        spr = Sprite()
        d=spr.texture.get_image_data()

        d1 = d.get_data('RGBA',d.width*4)
        for i in range(0,4096*4):
            d1[i] = random.randint(0,250)

        i = pyglet.image.ImageData(64,64,'RGBA',d1)

        while not self.has_exit:
            self.dispatch_events()
            self.clear()

            #Tick the clock
            clock.tick()
            #Gets fps and draw it
            fps_text.text = ("fps: %d") % (clock.get_fps())
            fps_text.draw()
            spr.draw()
            #i.blit(50,50)
            self.flip()
            spr.update()

if __name__ == "__main__":
    # Someone is launching this directly
    space = SpaceGameWindow()
    space.main_loop()
