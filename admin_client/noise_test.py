import sys
import os
sys.path.insert(0, './ext/')

import _Noise as N

from pyglet import window
from pyglet import clock
from pyglet import font
import pyglet

import time

class Sprite(object):
    def __init__(self):
        self.texture = pyglet.image.Texture.create(64,64, pyglet.gl.GL_RGBA)
        self.x = 250
        self.y = 150

        self.z = 0
        self.update()

    def draw(self):
        #self.image.blit(self.x, self.y)
        #self.texture.blit(self.x, self.y)
        self.image.blit(self.x,self.y)
        #self.image2.blit(self.x+128,self.y)
    def update(self):
        d = self.texture.get_image_data()
        d1 = d.get_data('RGBA',d.width*4)

        self.z += 0.1
        zoom = 5
        '''
        def F(a,b):
            return N._Vo(zoom*a/64.0,zoom*b/64.0,self.z, 4, 4)
        '''
        li=[]
        for i in range(0,4096):
            x = i % 64
            y = i / 64
            #l = 1+N._p3(zoom*x/64.0,zoom*y/64.0,self.z)
            l = N._Vo(zoom*x/64.0,zoom*y/64.0,self.z, 0, 0)
            #l = F(x,y)
            #l = F_tile(x,y,64.0,64.0)
            li.insert(i, l)
            l -= 0
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
        '''
        def F_tile(x,y,w,h):
            return (F(x, y) * (w - x) * (h - y) + F(x - w, y) * (x) * (h - y) + F(x - w, y - h) * (x) * (y) + F(x, y - h) * (w - x) * (y)) / (w * h)

        #d2 = d.get_data('RGBA',d.width*4)
        d2 = pyglet.image.Texture.create(64,64, pyglet.gl.GL_RGBA).get_image_data().get_data('RGBA',d.width*4)
        for i in range(0,4096):
            x = i % 64
            y = i / 64
            l = F_tile(x,y, 64.0, 64.0)
            l -= 0
            if l < 0:
                l=0
            if l > 1:
                l=1
            l = int(255*l)
            d2[4*i+0] = l
            d2[4*i+1] = l
            d2[4*i+2] = l
            d2[4*i+3] = 255
        self.image2 = pyglet.image.ImageData(64,64,'RGBA',d2)
        '''
    def save(self):
        self.image.save('out.png')

class SpriteT(object):
    def __init__(self):
        self.texture = pyglet.image.Texture.create(64,64, pyglet.gl.GL_RGBA)
        self.x = 150
        self.y = 150

        self.t = 0
        self.update()

    def draw(self):
        self.image.blit(self.x,self.y)

    def update(self):
        d = self.texture.get_image_data()
        d1 = d.get_data('RGBA',d.width*4)

        self.t += 0.005
        zoom = 1.0
        dx = 0.35
        dy = 0.35
        '''
        x0 = 0.0 + self.t
        y0 = 0.0
        x1 = x0 + dx
        y1 = y0 + dy
        '''
        x0 = random.random()
        y0 = random.random()
        x1 = random.random()
        y1 = random.random()

        #double p4(int x, int y, double x1, double y1, double x2, double y2, double height, double width)

        li=[]
        sum = 0.0
        for i in range(0,4096):
            x = i % 64
            y = i / 64
            l = N._p4T(x,y, x0,y0, x1,y1, 64.0, 64.0)
            li.insert(i,l)
            sum += l

        average = sum/4096.0
        print "average = %f" % (average)

        for i in range(0,4096):
            li[i] += 0.2

        d2 = pyglet.image.Texture.create(64,64, pyglet.gl.GL_RGBA).get_image_data().get_data('RGBA',d.width*4)
        for i in range(0,4096):
            x = i % 64
            y = i / 64
            '''
            l = N._p4T(x,y, x0,y0, x1,y1, 64.0, 64.0)

            #l += 0.4
            #print str(l)
            if l < 0:
                l=0
            if l > 1:
                l=1
            l = int(255*l)
            '''
            l = li[i]
            if l < 0:
                l=0
            if l > 1:
                l=1
            l = int(255*l)
            d2[4*i+0] = l
            d2[4*i+1] = l
            d2[4*i+2] = l
            d2[4*i+3] = 255
        self.image = pyglet.image.ImageData(64,64,'RGBA',d2)

    def save(self):
        self.image.save('out.png')


class Sprite32(object):
    def __init__(self):
        self.texture = pyglet.image.Texture.create(32,32, pyglet.gl.GL_RGBA)
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
        #zoom = 8.0
        zoom = 2.0
        def F(a,b):
            return N._Vo(zoom*a/32.0,zoom*b/32.0,self.z, 0, 0)
        def F_tile(x,y,w,h):
            return (F(x, y) * (w - x) * (h - y) + F(x - w, y) * (x) * (h - y) + F(x - w, y - h) * (x) * (y) + F(x, y - h) * (w - x) * (y)) / (w * h)
        for i in range(0,1024):
            x = i % 32
            y = i / 32
            #l = 1+N._p3(zoom*x/64.0,zoom*y/64.0,self.z)
            l = N._Vo(zoom*x/32.0,zoom*y/32.0,self.z, 0, 0)
            #l = F_tile(x,y,32,32)
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


        self.image = pyglet.image.ImageData(32,32,'RGBA',d1)
    def save(self):
        self.image.save('out32.png')


'''

F_tile(x, y, w, h) = (

F(x, y) * (w - x) * (h - y) +

F(x - w, y) * (x) * (h - y) +

F(x - w, y - h) * (x) * (y) +

F(x, y - h) * (w - x) * (y)

) / (w * h
'''

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
        #spr2 = SpriteT()
        #d=spr.texture.get_image_data()

        #d1 = d.get_data('RGBA',d.width*4)
        #for i in range(0,4096*4):
        #    d1[i] = random.randint(0,250)

        #i = pyglet.image.ImageData(64,64,'RGBA',d1)

        #spr.save()

        while not self.has_exit:
            self.dispatch_events()
            self.clear()

            #Tick the clock
            clock.tick()
            #Gets fps and draw it
            fps_text.text = ("fps: %d") % (clock.get_fps())
            fps_text.draw()


            spr.draw()
            #spr2.draw()
            #i.blit(50,50)
            self.flip()
            spr.update()
            #spr2.update()
            #time.sleep(0.500)

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
    if False:
        _S = Sprite32()
        _S.save()

    space = SpaceGameWindow()
    space.main_loop()
