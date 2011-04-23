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

def convert_index(index, height, width):
    index = int(index)
    height = int(height)
    width = int(width)
    x_ = index % width
    y_ = int((index - x_) / width)
    y = height - y_ -1
    rvalue =  x_ + y*width
    #print "rvalue= " + str(rvalue)
    return rvalue

import random


def elevation_to_color(element):
    if element < 0:
        return (0,0,63)
    elif element <= 0.05:
        return (0,0,191)
    elif element <= 0.1:
        return (0,0,255)
    elif element <= 0.15:
        return (114,114,255)
    elif element <= 0.2:
        return (165,165,255)
    elif element <= 0.25:
        return (191,191,255)
    elif element <= 0.3:
        return (255,232,165)
    elif element <= 0.35:
        return (191,255,191)
    elif element <= 0.4:
        return (165,255,165)
    elif element <= 0.45:
        return (114,255,114)
    elif element <= 0.5:
        return (0,255,0)
    elif element <= 0.55:
        return (0,191,0)
    elif element <= 0.6:
        return (0,127,0)
    elif element <= 0.65:
        return (0,63,0)
    elif element <= 0.7:
        return (63,50,31)
    elif element <= 0.75:
        return (31,24,15)
    elif element <= 0.8:
        return (63,63,63)
    elif element <= 0.85:
        return (127,127,127)
    elif element <= 0.9:
        return (159,159,159)
    elif element <= 0.95:
        return (191,191,191)
    else:
        return (255,255,255)
    pass

class World(object):

    def __init__(self):
        pass
        #texture loading
        #tile_image = pyglet.image.load('../texture/textures_01.png')
        #tile_image_grid = pyglet.image.ImageGrid(tile_image, 16, 16)
        #tile_texture_grid = pyglet.image.TextureGrid(tile_image_grid)
        #self.texture_grid = tile_texture_grid


        #self.terrainMap = TerrainMap()

    def tick(self):
        pass

    def setup(self, v_array):
        self.v_array = v_array

        v_list = []
        c_list = []
        v_num = 0
        for (x,y,z) in v_array:
            #print str((x,y,z))
            v_list += [x,y,30*z]

            if False:
                co = list(elevation_to_color(z))
                co.insert(4, 0)
                c_list += co
            else:
                c_list += [255, 255,0,0]

            v_num += 1
            #self.draw_point2(x, y, z, 255, 255,0)

        self.vertexList = pyglet.graphics.vertex_list(v_num,
            ('v3f\static', v_list),
            ('c4B\static', c_list),
        )
    def draw(self):
        #for x in range(-20, 20):
        #    self.draw_point(x, 0, 0, 255, 0)

        #for y in range(-20, 20):
        #    self.draw_point(0, y, 0, 0, 255)

        self.vertexList.draw(pyglet.gl.GL_POINTS)

    def draw_point(self, x, y, r, g, b):
        z=0
        pyglet.graphics.draw(1, pyglet.gl.GL_POINTS,
            ('v3f', (x, y, z)),
            ('c3B', (r, g, b)) )

    def draw_point2(self, x, y, z, r, g, b):
        pyglet.graphics.draw(1, pyglet.gl.GL_POINTS,
            ('v3f', (x, y, z)),
            ('c3B', (r, g, b)) )

from math import sin, cos, pi

class Camera(object):

    def __init__(self, win, x=0.0, y=0.0, z=0.0, rot=0.0, zoom=1.0):
        self.win = win
        self.x = x
        self.y = y
        self.z = z
        self.x_angle = 0.0
        self.y_angle = 0.0
        self.rot = rot
        self.zoom = zoom
        self.rts = True

    def worldProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        aspect = self.win.width / float(self.win.height)
        #gluPerspective( 45.0 / self.scale, aspect, 0.1, 100.0);
        gluPerspective(65, aspect, .1, 1000)


        glMatrixMode( GL_MODELVIEW )
        glLoadIdentity()

        camera_focus_x = self.x + cos( self.x_angle * pi)
        camera_focus_y = self.y + sin( self.x_angle * pi)
        camera_focus_z = self.z + sin( self.y_angle)

        gluLookAt( self.x, self.y, self.z,
                camera_focus_x, camera_focus_y, camera_focus_z,
                0., 0., 1.0)

        glEnable (GL_DEPTH_TEST)
        glEnable(GL_CULL_FACE);

    def hudProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluOrtho2D(0, self.win.width, 0, self.win.height)

    def move_camera(self, dx, dy, dz):

        if self.rts == True:
            #dx delta
            self.x += dx*cos(self.x_angle * pi)
            self.y += dx*sin(self.x_angle * pi)
            #dy delta
            self.x += dy*cos(self.x_angle * pi + pi/2)
            self.y += dy*sin(self.x_angle * pi + pi/2)
        else:
            self.x += dx
            self.y += dy
        #dz delta
        self.z += dz

    def pan(self, dx_angle, dy_angle):
        #print "dx_angle= " + str(dx_angle)
        #print "dy_angle= " + str(dy_angle)
        self.x_angle += dx_angle
        self.y_angle += dy_angle

    def on_resize(self, width, height):
        print "Resize Window"
        glViewport(0, 0, width, height)
        self.worldProjection()
        return pyglet.event.EVENT_HANDLED

class Hud(object):

    def __init__(self, win):
        helv = font.load('Helvetica', win.width / 15.0)
        self.text = font.Text(
            helv,
            'Hello, World!',
            x=win.width / 2,
            y=win.height / 2,
            halign=font.Text.CENTER,
            valign=font.Text.CENTER,
            color=(1, 1, 1, 0.5),
        )
        self.fps = clock.ClockDisplay()

    def draw(self):
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        #self.text.draw()
        self.draw_recticle()
        self.fps.draw()

    def draw_recticle(self):
        return
        x=win.width / 2,
        y=win.height / 2,

class Mouse(object):

    def __init__(self, main):
        self.main = main
        self.camera = main.camera

    def on_mouse_drag(self, x, y, dx, dy, buttons, modifiers):
        #print 'dy, dy = ' +  str(dx) + ' ' + str(dy)
        sen = 50
        self.camera.pan(dx*-1.0 / sen, dy*1.0 / sen)


from pyglet.window.key import symbol_string
from pyglet.window import key

class Keyboard(object):

    def __init__(self, main):
        self.main = main
        self.camera = main.camera
        self.key_handlers = {}

    def on_key_press(self, symbol, modifiers):
        v = 0.2

        if symbol == key.SPACE:
            print "Space Bar Pressed"
        #elif symbol == key.R:
            #self.camera.move_camera(0,0,v)
        #elif symbol == key.F:
            #self.camera.move_camera(0,0,-v)
        elif symbol in self.key_handlers:
            self.key_handlers[symbol]()

    def stateHandler(self, keyboard):
        v = 0.1
        if keyboard[key.W]:
            self.camera.move_camera(v,0,0)
        if keyboard[key.S]:
            self.camera.move_camera(-v,0,0)
        if keyboard[key.A]:
            self.camera.move_camera(0,v,0)
        if keyboard[key.D]:
            self.camera.move_camera(0,-v,0)
        if keyboard[key.R]:
            self.camera.move_camera(0,0,v)
        if keyboard[key.F]:
            self.camera.move_camera(0,0,-v)
        if keyboard[key.SPACE]:
            print "Event A.1"

class App(object):

    def __init__(self):

        self.world = World()
        self.win = window.Window(fullscreen=True, vsync=False)
        self.camera = Camera(self.win)
        self.camera = Camera(self.win)
        self.keyboard = Keyboard(self)
        self.mouse = Mouse(self)
        self.hud = Hud(self.win)
        #clock.set_fps_limit(60)
        #setup events
        self.keyboard.key_handlers[key.ESCAPE] = self.win.close
        self.win.on_mouse_drag = self.mouse.on_mouse_drag
        self.win.on_key_press = self.keyboard.on_key_press

        print "App init finished"

    def mainLoop(self, vertex_array):
        self.world.setup(vertex_array)
        clock.set_fps_limit(60)
        keyboard = key.KeyStateHandler()
        while not self.win.has_exit:
            self.win.push_handlers(keyboard)
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



#app = App()
#cProfile.run('app.mainLoop2()')
#app.mainLoop(vertex_array)

