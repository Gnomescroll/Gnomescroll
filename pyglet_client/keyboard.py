from sys import stdout
from pyglet.window.key import symbol_string
from pyglet.window import key

class Keyboard(object):

	def __init__(self, main, camera):
		self.camera = camera
		self.key_handlers = {}

#	def on_key_press(self, symbol, modifiers):
#		print "on key press: why is this running"
#		if symbol in self.key_handlers:
#			self.key_handlers[symbol]()
#		else:
#			print "on key press: cannot handle, error"


	def on_key_press(self, symbol, modifiers):
		v = 0.2
		
		if symbol == key.SPACE:
			print "Space Bar Pressed"
		elif symbol == key.W:
			self.camera.move_camera(v,0,0)
		elif symbol == key.S:
			self.camera.move_camera(-v,0,0)
		elif symbol == key.A:
			self.camera.move_camera(0,v,0)
		elif symbol == key.D:
			self.camera.move_camera(0,-v,0)
		elif symbol == key.R:
			self.camera.move_camera(0,0,v)
		elif symbol == key.F:
			self.camera.move_camera(0,0,-v)
		elif symbol in self.key_handlers:
			self.key_handlers[symbol]()

	def stateHandler(self, keyboard):
		if keyboard[key.UP]:
			print "Event up"
		if keyboard[key.A]:
			print "Event A.1"
		if keyboard[key.ESCAPE]:
			self.main.win.close
	
		def print_handlers(self):
			print "keys to try:", \
				[symbol_string(k) for k in self.key_handlers.keys()]
			stdout.flush()

