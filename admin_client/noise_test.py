import sys
import os
sys.path.insert(0, './ext/')

import _Noise as N

from pyglet import window
from pyglet import clock
from pyglet import font
import pyglet

class Sprite(object):
    def __init__(self):
        self.texture = pyglet.image.Texture.create(64,64, pyglet.gl.GL_RGBA)
        self.x = 150
        self.y = 150

        self.z = 0
        self.update()

    def draw(self):
        #self.image.blit(self.x, self.y)
        #self.texture.blit(self.x, self.y)
        self.image.blit(self.x,self.y)

    def update(self):
        d = self.texture.get_image_data()
        d1 = d.get_data('RGBA',d.width*4)

        self.z += 0.05
        zoom = 8.0
        for i in range(0,4096):
            x = i % 64
            y = i / 64
            #l = 1+N._p3(zoom*x/64.0,zoom*y/64.0,self.z)
            l = N._Vo(zoom*x/64.0,zoom*y/64.0,self.z, 0, 0)
            l -= .2
            if l < 0:
                l=0
            if l > 1:
                l=1
            #print(str(l))
            l = int(255*l)
            d1[4*i+0] = l
            d1[4*i+1] = l
            d1[4*i+2] = l
            d1[4*i+3] = 255

        self.image = pyglet.image.ImageData(64,64,'RGBA',d1)
    def save(self):
        self.image.save('out.png')

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

        spr.save()

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

import png
import sys
import os.path



def gen_perlin(time_step, zoom= 4, offset=-0.2):
    z = 0.1 * float(time_step)
    li = []

    x = 0.0
    for i in range(0,64):
        y = 0.0
        for j in range(0,64):
            l = 1+N._p3(zoom*x/64.0,zoom*y/64.0,z)
            l += offset
            if l < 0:
                l=0
            if l > 1:
                l=1
            li.insert(64*i+j, int(l*255))
            y += 1.0
        x += 1.0
    return li

def gen_voronoi(time_step, step_size=0.1, zoom= 8, offset=-0.2):
    z = step_size * float(time_step)
    li = []

    x = 0.0
    for i in range(0,64):
        y = 0.0
        for j in range(0,64):
            #l = 1+N._p3(zoom*x/64.0,zoom*y/64.0,z)
            l = N._Vo(zoom*x/64.0,zoom*y/64.0,z, 0, 0)
            l += offset
            if l < 0:
                l=0
            if l > 1:
                l=1
            li.insert(64*i+j, int(l*255))
            y += 1.0
        x += 1.0
    return li

class Spritesheet(object):
    def __init__(self):
        self._set_empty_pixels()

    def _set_empty_pixels(self):
        rows = []
        i2=0
        for i in range(0,1024):
            rows.insert(i,[])
            rows[i] = [0,0,0,0] * 1024
        self.pixels = rows

    def generate(self):
        print "Generating"
        def _offset(id):
            x = id % 16
            y = (id - x) / 16
            return (x,y)

        for id in range(0,256):
            #perlin generator here

            #_l = gen_perlin(time_step=id)
            _l = gen_voronoi(time_step=id, step_size=0.1, zoom= 8, offset=-0.0)
            xi,yi = _offset(id)
            i = 0
            j= 0
            for i in range(64):
                for j in range(64):
                    v = _l[64*i+j]
                    self.pixels[i+64*yi][4*(j+64*xi)+0] = v #r
                    self.pixels[i+64*yi][4*(j+64*xi)+1] = v #g
                    self.pixels[i+64*yi][4*(j+64*xi)+2] = v #b
                    self.pixels[i+64*yi][4*(j+64*xi)+3] = 255 #a
        return self.pixels

    def write_out(self, filename):
        print "Writing Out"
        pixels = self.pixels
        png_out = png.Writer(width=1024, height=1024, alpha=True, bitdepth=8, transparent=None)
        with open(filename, 'wb') as f:      # binary mode is important
            png_out.write(f, pixels)

if __name__ == "__main__":
    x = True
    if False:
        _S = Spritesheet()
        _S.generate()
        _S.write_out("simplex.png")

    space = SpaceGameWindow()
    space.main_loop()
