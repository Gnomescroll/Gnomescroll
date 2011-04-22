from pyglet.gl import *
import pyglet
from pyglet import clock

from math import cos, sin

class Main(object):

	def __init__(self):
		#configure window
		platform = pyglet.window.get_platform()
		display = platform.get_default_display()
		screen = display.get_default_screen()
		
		template = pyglet.gl.Config(alpha_size=8)
		config = screen.get_best_config(template)
		context = config.create_context(None)
		self.window = pyglet.window.Window(context=context)
		#event handlers
		self.window.on_resize = self.on_resize
		self.window.on_draw = self.draw
		
		self.zc = 0.01
		#pyglet.clock.set_fps_limit(60)
		#self.fps_display = clock.ClockDisplay()
		
	def on_resize(self, width, height):
		#return pyglet.event.EVENT_HANDLED
		print "Resize Window"
		glViewport(0, 0, width, height)
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity()
		gluPerspective(65, width / float(height), .1, 1000)
		glMatrixMode(GL_MODELVIEW)
		return pyglet.event.EVENT_HANDLED		
		
	def draw(self):
		
	#DRAW
		self.zc += .005
		zc  = self.zc
		
		print "zc= " + str(zc)
		
		self.window.dispatch_events()		
		self.window.clear()			
	##DRAW		
		glMatrixMode(GL_MODELVIEW)
		glLoadIdentity()
		
		cx = 10*sin(zc)
		cy = 10*cos(zc)
		
		zoom = 100.0
		print '(cx, cy)= %.2f, %.2f' % (cx, cy)
		gluLookAt( zoom*sin(zc), zoom*cos(zc), zoom,
			0, 0, 0,
			0., 0., 1.0)
			
		z = -4
		glClear(GL_COLOR_BUFFER_BIT)
		#glLoadIdentity()
		glBegin(GL_TRIANGLES)
		glVertex3f(0., 0., z)
		glVertex3f(self.window.width, 0, z)
		glVertex3f(self.window.width, self.window.height, z)
		glEnd()
	
		if False:
			pyglet.graphics.draw(2, pyglet.gl.GL_POINTS,
			('v3i', (0,0,-3, 1,1,-1)),
			('c3B', (0, 0, 255, 0, 255, 0))
			)
		if False:
			points = [(0,0,-1), (0,0,-5), (10,10, -2)]
			
			for p in points:
				pyglet.graphics.draw(1, pyglet.gl.GL_POINTS,
				('v3f', p),
				('c3B', (0, 255, 0))	
				)
		
		zz = 0
		for x in range(-20, 20):
			pyglet.graphics.draw(1, pyglet.gl.GL_POINTS,
			('v3f', (x, 0, zz)),
			('c3B', (0, 255, 0))	
			)
		for y in range(-20, 20):
			pyglet.graphics.draw(1, pyglet.gl.GL_POINTS,
			('v3f', (0, y, zz )),
			('c3B', (0, 0, 255))	
			)
		
		#self.fps_display.draw()
		
if __name__ == "__main__":
	app = Main()
	pyglet.app.run()
