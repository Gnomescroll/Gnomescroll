from math import cos, pi, sin

from pyglet.window import key
from pyglet.gl import *
import pyglet

class Target(object):

    def __init__(self, camera):
        self.x, self.y, self.z = camera.x, camera.y, camera.z
        self.size = camera.scale
        self.x_angle = camera.x_angle
        self.y_angle = camera.y_angle
        

class RTS_Camera:
    
    def __init__(self, win_width, win_height, position=None, scale=None, x_angle=None, y_angle=None):
        print "ASFDSFDS"
        self.win_width = win_width
        self.win_height = win_height
        if position is None:
            position = (0., 0., 0.)
        self.x, self.y, self.z = position
        if scale is None:
            scale = 1
        self.scale = scale
        if x_angle is None:
            x_angle = 0.
        self.x_angle = x_angle
        if y_angle is None:
            y_angle = 0.
        self.y_angle = y_angle
        self.target = Target(self)
        self.key_handlers = {}

        self.fps_label = pyglet.text.Label('FPS: %f' % pyglet.clock.get_fps(),
            font_name='Times New Roman', font_size=12,
            x= 10  , y=self.win_height, anchor_x='left', anchor_y='top')
        #gluPerspective( 45.0, 640.0 / 480.0, 0.1, 100.0);
           # key.PAGEUP: lambda: self.zoom(2),
           # key.PAGEDOWN: lambda: self.zoom(0.5),
           # key.LEFT: lambda: self.pan(self.scale, -pi/2),
           # key.RIGHT: lambda: self.pan(self.scale, +pi/2),
           # key.DOWN: lambda: self.pan(self.scale, pi),
            #key.UP: lambda: self.pan(self.scale, 0),
            #key.COMMA: lambda: self.tilt(-1),
            #key.PERIOD: lambda: self.tilt(+1),
            #}
    
    def on_resize(self, width, height):
        print "Resize Window"
        glViewport(0, 0, width, height)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(65, self.win_width / float(self.win_height), .1, 1000)
        glMatrixMode(GL_MODELVIEW)
        return pyglet.event.EVENT_HANDLED
    
    def focus(self):
        glMatrixMode(GL_PROJECTION)
        #glLoadIdentity()
        aspect = self.win_width / float(self.win_height)
        #gluPerspective( 45.0 / self.scale, aspect, 0.1, 100.0);
        gluPerspective(65, aspect, .1, 1000)
        
        glMatrixMode( GL_MODELVIEW )
        glLoadIdentity()

        camera_focus_x = self.x + cos( self.x_angle * pi);
        camera_focus_y = self.y + sin( self.x_angle * pi);
        camera_focus_z = self.z + sin( self.y_angle);
        #print "Camera Angle: (%s, %s)" % (str(self.x_angle/(2*pi)), str(self.y_angle/(2*pi)))
        #print "Camera Focus Vector: (%s, %s, %s)" % ( str(cos( self.x_angle * pi)), str(sin( self.x_angle * pi)), str(sin( self.y_angle)))
        
        #print 'context= ' + str(pyglet.gl.get_current_context())
        
        precision = 2
        camera = "camera= " + "(%.1f, %.1f, %.1f), (%.1f, %.1f, %.1f)" % (
        round(self.x, precision), round(self.y,precision), round(self.z,precision),
        round(camera_focus_x- self.x, precision), round(camera_focus_y-self.y, precision), round(camera_focus_z-self.z, precision)
        )
        print camera
        
        print "self.x= " + str(self.x)
        
        gluLookAt( self.x, self.y, self.z,
                camera_focus_x, camera_focus_y, camera_focus_z,
                0., 0., 1.0)

    def zoom(self, factor):
        self.target.size *= factor

    def pan(self, dx_angle, dy_angle):
        #print "dx_angle= " + str(dx_angle)
        #print "dy_angle= " + str(dy_angle)
        self.x_angle += dx_angle
        self.y_angle += dy_angle

    def move_camera(self, dx, dy, dz):
        self.x += dx
        self.y += dy
        self.z += dz

    def update_position(self):
        "Quickly but smoothly move/zoom/tilt towards our target"
        self.x += (self.target.x - self.x) * 0.1
        self.y += (self.target.y - self.y) * 0.1
        self.z += (self.target.z - self.z) * 0.1
        self.scale += (self.target.size * 2 - self.scale) * 0.1
        self.x_angle += (self.target.x_angle - self.x_angle) * 0.1
        self.y_angle += (self.target.y_angle - self.y_angle) * 0.1


    def hud_mode(self):
        "Set matrices ready for drawing HUD, like fps counter"
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        #gluOrtho(0, self.win_width, 0, self.win_height, -1, 1)
        gluOrtho2D(0, self.win_width, 0, self.win_height)
        glMatrixMode(GL_MODELVIEW)
        gluLookAt(
            0, 0, +1.0,
            0, 0, -1.0,
            1, 0, 0.0)


        pyglet.graphics.draw(1, pyglet.gl.GL_POINTS,
        ('v2f', (5, 50)),
        ('c3B', (0, 256, 0)) )

        pyglet.graphics.draw(1, pyglet.gl.GL_POINTS,
        ('v2f', (0, 0)),
        ('c3B', (0, 256, 0)) )

    def draw_fps(self, dt):
        #self.fps_label.text = 'FPS: %f dt: %f' % (fps, dt)
        self.fps_label.text = 'FPS: %f dt: %f' % (pyglet.clock.get_fps(), dt)
        #self.fps_label.text = 'FPS: %f' % pyglet.clock.get_fps()
        self.fps_label.draw()   
        #print "frame draw"
        
    def draw_camera_indicator(self):
        pyglet.graphics.draw(1, pyglet.gl.GL_POINTS,
        ('v2f', (self.x_angle, 50)),
        ('c3B', (256, 256, 0)) )

