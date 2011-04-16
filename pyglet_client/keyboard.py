from sys import stdout
from pyglet.window.key import symbol_string

class Keyboard(object):

    def __init__(self):
        self.key_handlers = {}

    def on_key_press(self, symbol, modifiers):
        if symbol in self.key_handlers:
            self.key_handlers[symbol]()

    def print_handlers(self):
        print "keys to try:", \
            [symbol_string(k) for k in self.key_handlers.keys()]
        stdout.flush()

