from math import sin, cos, pi

from pyglet.gl import *
from pyglet import clock, font, image, window

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

        #camera_focus_x = self.x + cos( self.x_angle * pi)
        #camera_focus_y = self.y + sin( self.x_angle * pi)
        #camera_focus_z = self.z + sin( self.y_angle)

        camera_focus_x = self.x + cos( self.x_angle * pi) * cos( self.y_angle * pi)
        camera_focus_y = self.y + sin( self.x_angle * pi) * cos( self.y_angle * pi)
        camera_focus_z = self.z + sin( self.y_angle)

        gluLookAt( self.x, self.y, self.z,
                camera_focus_x, camera_focus_y, camera_focus_z,
                0., 0., 1.0)

        glEnable (GL_DEPTH_TEST)
        #glEnable(GL_CULL_FACE);

    def hudProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluOrtho2D(0, self.win.width, 0, self.win.height)

        glMatrixMode( GL_MODELVIEW )
        glLoadIdentity()

        glDisable(GL_DEPTH_TEST);
        #glDisable(GL_CULL_FACE);


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
        if self.y_angle < -0.499:
            self.y_angle = -0.499
        if self.y_angle > 0.499:
            self.y_angle = 0.499

    def on_resize(self, width, height):
        print "Resize Window"
        glViewport(0, 0, width, height)
        self.worldProjection()
        return pyglet.event.EVENT_HANDLED

class Hud(object):

    def __init__(self, win):
        self.win = win
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
        self._init_reticle()

    def _init_reticle(self):
        self.reticle = pyglet.image.load('./target.png')
        self.reticle_texture = self.reticle.get_texture()

        rh = 16.
        rw = 16.

        w = float(self.win.width)/2
        h = float(self.win.height)/2
        print str((h,w))
        x_min = w - rw/2
        x_max = w + rw/2
        y_min = h - rh/2
        y_max = h + rh/2

        v_list = [
        x_min, y_max,
        x_max, y_max,
        x_max, y_min,
        x_min, y_min
        ]
        print str(v_list)

        self.reticleVertexList = pyglet.graphics.vertex_list(4,
            ('v2f\static', v_list),
            ('c3B\static', (255,255,255) *4),
            ("t3f\static", self.reticle_texture.tex_coords),
            )

    def draw(self):
        #self.text.draw()
        self.draw_reticle()
        self.fps.draw()

    def draw_reticle(self):
        glEnable(GL_TEXTURE_2D)        # typically target is GL_TEXTURE_2D
        glBindTexture(self.reticle_texture.target, self.reticle_texture.id)

        glEnable (GL_BLEND)
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        self.reticleVertexList.draw(pyglet.gl.GL_QUADS)
        glDisable (GL_BLEND);
