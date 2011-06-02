#!/usr/bin/python

'''
Client input
'''



from pyglet.window import key
from pyglet.window import mouse
from pyglet.window.key import symbol_string

from sounds import playSound

class InputGlobal:
    keyboard = None
    mouse = None

    input = 'camera'
    _inputs = ('camera', 'agent')
    camera = 'camera'
    _cameras = ('camera', 'agent')

    @classmethod
    def init_0(self, main):
        InputGlobal.mouse = Mouse(main)
        InputGlobal.keyboard = Keyboard(main)
        InputGlobal.agentInput = AgentInput()

    @classmethod
    def init_1(self, main):
        InputGlobal.keyboard.bind_key_handlers(key.ESCAPE, main._exit)

    @classmethod
    def _toggle_mode(self, change, current_mode, type):
        modes = getattr(InputGlobal, '_'+type+'s')
        current_mode = (current_mode + change) % len(modes)
        setattr(InputGlobal, type, modes[current_mode])
        print "%s mode= %s" % (type, str(getattr(InputGlobal, type)),)
        return current_mode

    @classmethod
    # toggles through modes.
    def toggle_input_mode(self, change=1, current_mode=[0]):
        current_mode[0] = InputGlobal._toggle_mode(change, current_mode[0], 'input')

    @classmethod
    def toggle_camera_mode(self, change=1, current_mode=[0]):
        current_mode[0] = InputGlobal._toggle_mode(change, current_mode[0], 'camera')

    @classmethod
    def enable_chat(self):
        InputGlobal.input = 'chat'

class Mouse(object):

    def __init__(self, main):
        self.main = main
        self.main.win.on_mouse_drag = self.on_mouse_drag
        self.main.win.on_mouse_motion = self.on_mouse_motion
        self.main.win.on_mouse_press = self.on_mouse_press
        self.main.win.on_mouse_scroll = self.on_mouse_scroll
        self.camera = main.camera

    def on_mouse_drag(self, x, y, dx, dy, buttons, modifiers):
        #if InputGlobal.input == 'agent':
        #    self._pan_agent(x, y, dx, dy)
        if InputGlobal.input == 'camera':
            self._pan_camera(x, y, dx, dy)

    def on_mouse_motion(self, x, y, dx, dy):
        if InputGlobal.input == 'agent':
            self._pan_agent(x, y, dx, dy, sen=90)

    def _pan_agent(self, x, y, dx, dy, sen=50):
        GameStateGlobal.agent.pan(dx*-1.0 / sen, dy*1.0 / sen)

    def _pan_camera(self, x, y, dx, dy, sen=50):
        self.camera.pan(dx*-1.0 / sen, dy*1.0 / sen)

    def on_mouse_press(self, x, y, buttons, modifiers):
        if InputGlobal.input == 'agent':
            if buttons == 1:
                #playSound.build()
                GameStateGlobal.agent.fire()
            elif buttons == 2:
                #playSound.music()
                pass
            elif buttons == 4:
                GameStateGlobal.agent.set_active_block()

    def on_mouse_scroll(self, x, y, scroll_x, scroll_y):
        if scroll_y > 0:
            GameStateGlobal.agent.switch_weapon('up')
        elif scroll_y < 0:
            GameStateGlobal.agent.switch_weapon('down')

import math
from math import sin, cos, pi

class Keyboard(object):

    def __init__(self, main):
        self.main = main
        self.main.win.on_key_press = self.on_key_press
        self.main.win.on_text = self.on_text #key input
        self.main.win.on_text_motion = self.on_text_motion #text movement

        self.camera = main.camera
        self.key_handlers = {}

        self._init_key_handlers()

    def _input_callback(self, callback):
        if callable(callback):
            callback(self)

    #key input
    def on_text(self, text):
        if InputGlobal.input == 'chat':
            callback = ChatClientGlobal.chatClient.input.on_text(text)
            self._input_callback(callback)
        else:
            if text == 'y':
                self.toggle_chat()

    # continuous non-character key detection
    #e.g. back space, cursor movement
    def on_text_motion(self, motion):
        if InputGlobal.input == 'chat':
            callback = ChatClientGlobal.chatClient.input.on_text_motion(motion)
            self._input_callback(callback)

    # one-time non character key detection
    # e.g. enter
    def on_key_press(self, symbol, modifiers):
        if InputGlobal.input == 'chat':
            callback = ChatClientGlobal.chatClient.input.on_key_press(symbol, modifiers)
            self._input_callback(callback)
        else:
            if InputGlobal.input == 'agent':
                InputGlobal.agentInput.on_key_press(symbol, modifiers)
            self.key_handlers.get(symbol, lambda: None)()

    def _init_key_handlers(self):
        self.bind_key_handlers({
            key.G : self.main.world.toggle_mipmap,
            key.T : self.main.world.toggle_gl_smooth,
            key.Q : InputGlobal.toggle_input_mode,
            key.E : InputGlobal.toggle_camera_mode,
        })

    # accept key,handler or a dict of key,handlers
    def bind_key_handlers(self, key, handler=None):
        if handler is None:
            assert type(key) == dict
            for k, h in key.items():
                self.key_handlers[k] = h
        else:
            self.key_handlers[key] = handler

    def toggle_chat(self):
        if InputGlobal.input == 'chat':
            InputGlobal.toggle_input_mode(0)
        else:
            InputGlobal.input = 'chat'

    # called in main game loop
    def stateHandler(self, keyboard):
        if InputGlobal.input == 'chat':
            return
        if InputGlobal.input == 'camera':
            self.camera_input_mode(keyboard)
        elif InputGlobal.input == 'agent':
            self.agent_input_mode(keyboard)

    def agent_input_mode(self, keyboard):
        if GameStateGlobal.agent.dead:
            return
        v = 1
        d_x, d_y, v_x, v_y, jetpack, jump, brake = [0 for i in range(7)]

        if keyboard[key.W]:
                v_x += v*cos( GameStateGlobal.agent.x_angle * pi)
                v_y += v*sin( GameStateGlobal.agent.x_angle * pi)
        if keyboard[key.S]:
                v_x += -v*cos( GameStateGlobal.agent.x_angle * pi)
                v_y += -v*sin( GameStateGlobal.agent.x_angle * pi)
        if keyboard[key.A]:
                v_x += v*cos( GameStateGlobal.agent.x_angle * pi + pi/2)
                v_y += v*sin( GameStateGlobal.agent.x_angle * pi + pi/2)
        if keyboard[key.D]:
                v_x += -v*cos( GameStateGlobal.agent.x_angle * pi + pi/2)
                v_y += -v*sin( GameStateGlobal.agent.x_angle * pi + pi/2)
        if keyboard[key.C]:
            brake = 1
        if keyboard[key.SPACE]:
            jetpack = 1
        GameStateGlobal.agent.control_state = [
            d_x,
            d_y,
            v_x,
            v_y,
            jetpack,
            jump,
            brake
        ]
        ## send control state to server
        #NetOut.sendMessage.send_agent_control_state(GameStateGlobal.agent.id, *GameStateGlobal.agent.control_state)

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
            pass

class AgentInput:

    def __init__(self):
        self.key_handlers = {}
        self._init_key_handlers()

    def _init_key_handlers(self):
        self._bind_key_handlers({
            key.R : self.reload,
            key._1: self.switch_weapon,
            key._2: self.switch_weapon,
            key._3: self.switch_weapon,
            key._4: self.switch_weapon,
            key._5: self.switch_weapon,
            key._6: self.switch_weapon,
            key._7: self.switch_weapon,
            key._8: self.switch_weapon,
            key._9: self.switch_weapon,
            key._0: self.switch_weapon,
        })

    # accept key,handler or a dict of key,handlers
    def _bind_key_handlers(self, key, handler=None):
        if handler is None:
            assert type(key) == dict
            for k, h in key.items():
                self.key_handlers[k] = h
        else:
            self.key_handlers[key] = handle

    def on_key_press(self, symbol, modifiers):
        self.key_handlers.get(symbol, lambda x,y: None)(symbol, modifiers)

    def reload(self, symbol, modifiers):
        print 'reloading'
        GameStateGlobal.agent.reload()

    def switch_weapon(self, symbol, modifiers):
        print 'switch weapon'
        print symbol, modifiers
        print key.symbol_string(symbol)
        try:
            weapon_index = int(key.symbol_string(symbol)[1:])
        except (ValueError, TypeError):
            return
        print 'attempting to switch weapon to ', weapon_index
        GameStateGlobal.agent.switch_weapon(weapon_index)


from game_state import GameStateGlobal
from chat_client import ChatClientGlobal
from net_out import NetOut
