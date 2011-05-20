#!/usr/bin/python

'''
Client input
'''

from world_state import WorldStateGlobal

class Mouse(object):

    def __init__(self, main):
        self.main = main
        self.main.win.on_mouse_drag = self.on_mouse_drag
        self.camera = main.camera
        self.player = WorldStateGlobal.player

    def on_mouse_drag(self, x, y, dx, dy, buttons, modifiers):
        #print 'dy, dy = ' +  str(dx) + ' ' + str(dy)
        sen = 50
        if buttons == 4:
            self.player.pan(dx*-1.0 / sen, dy*1.0 / sen)
        else:
            self.camera.pan(dx*-1.0 / sen, dy*1.0 / sen)


from pyglet.window.key import symbol_string
from pyglet.window import key

from chat_client import ChatClientGlobal

import math
from math import sin, cos, pi

class Keyboard(object):

    def __init__(self, main):
        self.main = main
        self.main.win.on_key_press = self.on_key_press
        self.camera = main.camera
        self.key_handlers = {}

        self.player = WorldStateGlobal.player
        self.toggle_input_mode()
        self._init_key_handlers()

    def on_text(self, text):
        if not self.mode == 'chat':
            return
        print "Key= " + str(text)
        callback = ChatClientGlobal.chatClient.input.process_key(text)
        if callable(callback):
            callback(self)

    def _init_key_handlers(self):
        self.bind_key_handlers({
            key.E : self.main.world.toggle_mipmap,
            key.T : self.main.world.toggle_gl_smooth,
            key.Y : self.toggle_chat
        })

    # accept key,handler or a dict of key,handlers
    def bind_key_handlers(self, key, handler=None):
        if handler is None:
            assert type(key) == dict
            for k, h in key.items():
                self.key_handlers[k] = h
        else:
            self.key_handlers[key] = handler

    # pyglet.window.on_key_press
    def on_key_press(self, symbol, modifiers):
        if self.mode == 'chat':
            pass
#            callback = ChatClientGlobal.chatClient.input.process(key, symbol, modifiers)
#            if callable(callback):
#                callback(self)
        else:
            self.key_handlers.get(symbol, lambda: None)()
            #self.stateHandler(symbol, modifiers)


    def toggle_chat(self):
        if self.mode == 'chat':
            self.toggle_input_mode(0)
        else:
            self.mode = 'chat'

    # toggles through modes.
    def toggle_input_mode(self, change=1 ,current_mode = [0]):
        modes = ('camera', 'agent')
        if getattr(self, 'mode', None) is not None:
            current_mode[0] = (current_mode[0] + change) % len(modes)
        self.mode = modes[current_mode[0]]

    # called in main game loop
    def stateHandler(self, keyboard):
        if self.mode == 'chat':
            return
        #mode switch
        if keyboard[key.Q]:
            self.toggle_input_mode()

        if self.mode == 'camera':
            self.camera_input_mode(keyboard)
        elif self.mode == 'agent':
            self.agent_input_mode(keyboard)

    def agent_input_mode(self, keyboard):
        # d_x=1, d_y=0, d_xa=0, d_za=0, jetpack=0, brake=0)
        v = 1
        [d_x, d_y, d_xa, d_za, jetpack, brake] = [0,0,0,0,0,0]

        if keyboard[key.W]:
                d_x += v*cos( self.player.x_angle * pi)
                d_y += v*sin( self.player.x_angle * pi)
        if keyboard[key.S]:
                d_x += -v*cos( self.player.x_angle * pi)
                d_y += -v*sin( self.player.x_angle * pi)
        if keyboard[key.A]:
                d_x += -v*cos( self.player.x_angle * pi + pi/2)
                d_y += -v*sin( self.player.x_angle * pi + pi/2)
        if keyboard[key.D]:
                d_x += v*cos( self.player.x_angle * pi + pi/2)
                d_y += v*sin( self.player.x_angle * pi + pi/2)
        if keyboard[key.E]:
            brake = 1
        if keyboard[key.SPACE]:
            jetpack = 1
        self.player.control_state = [
            d_x,
            d_y,
            d_xa,
            d_za,
            jetpack,
            brake
        ]

    def camera_input_mode(self, keyboard):
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

