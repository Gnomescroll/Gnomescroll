#! python -O

import pyglet
from pyglet import app, clock
from pyglet.window import key
from rts_camera import RTS_Camera
from mouse import Mouse
from keyboard import Keyboard
from renderer import Renderer

#@window.event
#def on_mouse_drag(x, y, dx, dy, buttons, modifiers):
#	global camera_x_angle, camera_y_angle
#	print 'dy, dy = ' +  str(dx) + ' ' + str(dy)
#	camera_x_angle = camera_x_angle + dx/100
#	camera_z_angle = camera_y_angle + dx/100

class App(object):

	def __init__(self):
		#configure window
		platform = pyglet.window.get_platform()
		display = platform.get_default_display()
		screen = display.get_default_screen()

		template = pyglet.gl.Config(alpha_size=8)
		config = screen.get_best_config(template)
		context = config.create_context(None)
		window = pyglet.window.Window(context=context)
		win = window
		#self.win = window.window(fullscreen=False)
        #self.camera = Camera(self.win, zoom=100.0)
        #self.hud = Hud(self.win)
        #clock.set_fps_limit(60)

		camera = RTS_Camera(win.width, win.height, (0, 0, 0))
	    #self.hud = Hud(self.win)
		renderer = Renderer()
		keyboard = Keyboard()
		mouse = Mouse(camera)
		keyboard.key_handlers[key.ESCAPE] = win.close
	    #self.keyboard.key_handlers.update(self.camera.key_handlers) #use to add handlers
	    #self.win.on_draw = lambda: renderer.on_draw(camera, win.width, win.height)
		win.on_mouse_drag = mouse.on_mouse_drag
		win.on_key_press = keyboard.on_key_press
	    #self.keyboard.print_handlers()
		win.set_visible()
	    
		self.camera = camera
		self.win = win
		self.renderer = renderer
		self.keyboard = keyboard
		self.mouse = mouse
	    
	def main(self):
		while not self.win.has_exit:
			self.win.dispatch_events()
			#self.world.tick()

			self.camera.focus()
			#self.camera.worldProjection()
			#self.world.draw()

			#self.camera.hudProjection()
			#self.hud.draw()

			clock.tick()
			self.win.flip()
    


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

if __name__ == "__main__":
	app = App()
	app.main()	
