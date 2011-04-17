from sys import stdout
from pyglet.window.key import symbol_string

class Keyboard(object):

    def __init__(self, app):
		self.camera = app.camera
        #self.key_handlers = {}

#    def on_key_press(self, symbol, modifiers):
#        if symbol in self.key_handlers:
#            self.key_handlers[symbol]()


def on_key_press(symbol, modifiers):
	v = 0.2
	
	if symbol == key.SPACE:
		"Space Bar Pressed"
	elif symbol == key.W:
		self.camera.move(
	elif symbol == key.S:
		pass
	elif symbol == key.A:
		print "Event A.0"
	elif symbol == key.D:
		pass
        
def stateHandler(self, keyboard):
	if keyboard[key.UP]:
		print "Event up"
	if keyboard[key.A]:
		print "Event A.1"
		
	def print_handlers(self):
		print "keys to try:", \
			[symbol_string(k) for k in self.key_handlers.keys()]
		stdout.flush()

