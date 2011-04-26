from __future__ import division
from random import uniform

from pyglet import clock, font, image, window
from pyglet.gl import *


class Entity(object):

    def __init__(self, id, size, x, y, rot):
        self.id = id
        self.size = size
        self.x = x
        self.y = y
        self.rot = rot

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


class World(object):

    def __init__(self):
        self.ents = {}
        self.nextEntId = 0
        clock.schedule_interval(self.spawnEntity, 0.25)

    def spawnEntity(self, dt):
        size = uniform(1.0, 100.0)
        x = uniform(-100.0, 100.0)
        y = uniform(-100.0, 100.0)
        rot = uniform(0.0, 360.0)
        ent = Entity(self.nextEntId, size, x, y, rot)
        self.ents[ent.id] = ent
        self.nextEntId += 1
        return ent

    def tick(self):
        for ent in self.ents.values():
            ent.rot += 10.0 / ent.size

    def draw(self):
        glClear(GL_COLOR_BUFFER_BIT)
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        for ent in self.ents.values():
            ent.draw()


class Camera(object):

    def __init__(self, win, x=0.0, y=0.0, rot=0.0, zoom=1.0):
        self.win = win
        self.x = x
        self.y = y
        self.rot = rot
        self.zoom = zoom

    def worldProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        widthRatio = self.win.width / self.win.height
        gluOrtho2D(
            -self.zoom * widthRatio,
            self.zoom * widthRatio,
            -self.zoom,
            self.zoom)

    def hudProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluOrtho2D(0, self.win.width, 0, self.win.height)


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
        self.text.draw()
        self.fps.draw()


class App(object):

    def __init__(self):
        self.world = World()
        self.win = window.Window(fullscreen=False, vsync=True)
        self.camera = Camera(self.win, zoom=100.0)
        self.hud = Hud(self.win)
        clock.set_fps_limit(60)

    def mainLoop(self):
        while not self.win.has_exit:
            self.win.dispatch_events()

            self.world.tick()

            self.camera.worldProjection()
            self.world.draw()

            self.camera.hudProjection()
            self.hud.draw()

            clock.tick()
            self.win.flip()

app = App()
app.mainLoop()

