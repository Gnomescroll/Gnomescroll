#!/usr/bin/python

'''
Client input
'''

import settings

if settings.pyglet:
    from pyglet.window import key
    from pyglet.window import mouse
    from pyglet.window.key import symbol_string
else:
    import SDL.input

#handles special characters
Keystring = {}
def setup_keystring():
    global Keystring

    special_keys = {
    'TAB': 9,
    'SPACE': 32,
    'LEFT': 276,
    'RIGHT': 275,
    'UP' : 273,
    'DOWN' : 274,
    'LSHIFT' : 304,
    'RSHIT' : 303,
    'LCTRL' : 306,
    'RCTRL' : 305,
    }
    for i in range(1,255):
        Keystring[i] = chr(i)
    for key,value in special_keys.items():
        Keystring[value] = key
setup_keystring()

from sounds import playSound

class InputEventGlobal:
    mouse = None
    keyboard = None

    #deprecate
    def keyboard_event(self, keycode):
        print str(keycode)
        key = Keystring.get(keycode, None)
        print 'keyboard event:'+ str(key)
        self.keyboard.on_key_press(key)
        if key == None:
            print "keycode unhandled= " + str(keycode)

    def keyboard_state(self, pressed_keys):
        keyboard = []
        for keycode in pressed_keys:
            temp = Keystring.get(keycode, None)
            if temp != None:
                keyboard.append(temp)
        self.keyboard.stateHandler(keyboard)

    #has issues with special characters
    def keyboard_text_event(self, keycode, key_string, state=0): #keystring is null
        key = Keystring.get(keycode, None)
        print "Text event, key_string=" + str(key_string) + " keycode=" + str(keycode) + " key= " + str(key)
        #self.keyboard.on_text(key_string)
        if state == 0:
            self.keyboard.on_key_press(key)
        else:
            self.keyboard.on_key_release(key)
        #print "text= " + key_string

    def mouse_event(self, button,state,x,y,):
        #handle scroll events
        #print str (button) + " " + str(state)

        self.mouse.on_mouse_press(x,y,button, state)
        #print "click"

    def mouse_motion(self, x,y,dx,dy,button):
        if button != 0:
            self.mouse.on_mouse_drag(x,y,dx,dy,button)
        else:
            self.mouse.on_mouse_motion(x,y,dx,dy)
            #print "motion"
        pass

class InputGlobal:
    keyboard = None
    mouse = None
    agentInput = None

    input = 'camera'
    _inputs = ('camera', 'agent')
    camera = 'camera'
    _cameras = ('camera', 'agent')

    scoreboard = False

    @classmethod
    def init_0(cls, main):
        #InputEventGlobal.inputGlobal = cls

        InputGlobal.mouse = Mouse(main)
        InputGlobal.keyboard = Keyboard(main)
        InputGlobal.agentInput = AgentInput()

        InputEventGlobal.mouse = cls.mouse
        InputEventGlobal.keyboard = cls.keyboard

    @classmethod
    def init_1(cls, main):
        if settings.pyglet: #deprecate!!
            InputGlobal.keyboard.bind_key_handlers(key.ESCAPE, main._exit)

    @classmethod
    def _toggle_mode(cls, change, current_mode, type):
        modes = getattr(InputGlobal, '_'+type+'s')
        current_mode = (current_mode + change) % len(modes)
        new_mode_name = modes[current_mode]
        if new_mode_name == 'agent' and GameStateGlobal.agent is None:
            return
        setattr(InputGlobal, type, new_mode_name)
        print "%s mode= %s" % (type, str(getattr(InputGlobal, type)),)
        return current_mode

    @classmethod
    # toggles through modes.
    def toggle_input_mode(cls, change=1, current_mode=[0]):
        current_mode[0] = InputGlobal._toggle_mode(change, current_mode[0], 'input')

    @classmethod
    def toggle_camera_mode(cls, change=1, current_mode=[0]):
        current_mode[0] = InputGlobal._toggle_mode(change, current_mode[0], 'camera')

    @classmethod
    def enable_chat(cls):
        InputGlobal.input = 'chat'

class Mouse(object):

    def __init__(self, main):
        self.main = main
        self.camera = main.camera

        if settings.pyglet:
            self.main.win.on_mouse_drag = self.on_mouse_drag
            self.main.win.on_mouse_motion = self.on_mouse_motion
            self.main.win.on_mouse_press = self.on_mouse_press
            self.main.win.on_mouse_scroll = self.on_mouse_scroll

    #inplement draw detection...
    def on_mouse_drag(self, x, y, dx, dy, buttons, modifiers=None):
        pass
        #if InputGlobal.input == 'agent':
        #    self._pan_agent(x, y, dx, dy)

    def on_mouse_motion(self, x, y, dx, dy):
        if InputGlobal.input == 'agent':
            self._pan_agent(x, y, dx, dy, sen=90)
        if InputGlobal.input == 'camera':
            self._pan_camera(x, y, dx, dy)

    def _pan_agent(self, x, y, dx, dy, sen=50):
        GameStateGlobal.agent.pan(dx*-1.0 / sen, dy*1.0 / sen)

    def _pan_camera(self, x, y, dx, dy, sen=50):
        self.camera.pan(dx*-1.0 / sen, dy*1.0 / sen)

    #buttonss:
    #1 left, 2 right, 4 scroll up, 5 scroll down
    #state is 0 or 1, 1 if mouse was click, 0 if it was released
    def on_mouse_press(self, x, y, button, state= None):
        if InputGlobal.input == 'agent':
            if state == 1: #pressed down
                if button == 1:
                    #playSound.build()
                    #print "fire"
                    GameStateGlobal.agent.fire()
                elif button == 3: #right click
                    GameStateGlobal.agent.set_active_block()
                elif button == 4: #scroll up
                    direction = 'up'
                    GameStateGlobal.agent.weapons.switch(direction)
                elif button == 5: #scroll down
                    direction = 'down'
                    GameStateGlobal.agent.weapons.switch(direction)
            elif state == 0: #mouse button released
                pass

    #migrate over
    ## Deprecate
#    def on_mouse_scroll(self, x, y, scroll_x, scroll_y):
#        if scroll_y > 0:
#            direction = 'up'
#        elif scroll_y < 0:
#            direction = 'down'
#        GameStateGlobal.agent.weapons.switch(direction)

from math import sin, cos, pi

class Keyboard(object):

    def __init__(self, main):
        self.main = main
        if settings.pyglet:
            self.main.win.on_key_press = self.on_key_press
            self.main.win.on_key_release = self.on_key_release
            self.main.win.on_text = self.on_text #key input
            self.main.win.on_text_motion = self.on_text_motion #text movement

        self.camera = main.camera
        self.key_handlers = {}

        self._init_key_handlers()

    def _input_callback(self, callback):
        if callable(callback):
            callback(self)

    #deprecate
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
    def on_key_press(self, symbol):
        if InputGlobal.input == 'chat':
            callback = ChatClientGlobal.chatClient.input.on_text(symbol)
            #callback = ChatClientGlobal.chatClient.input.on_key_press(symbol, modifiers)
            self._input_callback(callback)
        else:
            if symbol == 'y':
                self.toggle_chat()
            if InputGlobal.input == 'agent':
                InputGlobal.agentInput.on_key_press(symbol)
                #self.key_handlers.get(symbol, lambda: None)()
            if symbol == 'TAB':
                InputGlobal.scoreboard = True
            self.key_handlers.get(symbol, lambda x: None)(symbol)

    def on_key_release(self, symbol):
        if symbol == 'TAB':
            InputGlobal.scoreboard = False

    #deprecate for non-pyglet input
    def _init_key_handlers(self):
        if settings.pyglet:
            self.bind_key_handlers({
            key.G : self.main.world.toggle_mipmap,
            key.T : self.main.world.toggle_gl_smooth,
            key.Q : InputGlobal.toggle_input_mode,
            key.E : InputGlobal.toggle_camera_mode,
        })
        else:
            self.bind_key_handlers({
            "G" : self.main.world.toggle_mipmap,
            "T" : self.main.world.toggle_gl_smooth,
            "q" : InputGlobal.toggle_input_mode,
            "E" : InputGlobal.toggle_camera_mode,
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

        if settings.pyglet:
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
        else:
            if 'w' in keyboard:
                    v_x += v*cos( GameStateGlobal.agent.x_angle * pi)
                    v_y += v*sin( GameStateGlobal.agent.x_angle * pi)
            if 's' in keyboard:
                    v_x += -v*cos( GameStateGlobal.agent.x_angle * pi)
                    v_y += -v*sin( GameStateGlobal.agent.x_angle * pi)
            if 'a' in keyboard:
                    v_x += v*cos( GameStateGlobal.agent.x_angle * pi + pi/2)
                    v_y += v*sin( GameStateGlobal.agent.x_angle * pi + pi/2)
            if 'd' in keyboard:
                    v_x += -v*cos( GameStateGlobal.agent.x_angle * pi + pi/2)
                    v_y += -v*sin( GameStateGlobal.agent.x_angle * pi + pi/2)
            if 'c' in keyboard:
                brake = 1
            if 'SPACE' in keyboard:
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

        if settings.pyglet:
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
        else:
            if 'w' in keyboard:
                self.camera.move_camera(v,0,0)
            if 's' in keyboard:
                self.camera.move_camera(-v,0,0)
            if 'a' in keyboard:
                self.camera.move_camera(0,v,0)
            if 'd' in keyboard:
                self.camera.move_camera(0,-v,0)
            if 'r' in keyboard:
                self.camera.move_camera(0,0,v)
            if 'f' in keyboard:
                self.camera.move_camera(0,0,-v)
            #if keyboard[key.SPACE]:
            #    pass

class AgentInput:

    def __init__(self):
        self.key_handlers = {}
        self._init_key_handlers()

    def _init_key_handlers(self):
        if settings.pyglet:
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
        else:
            self._bind_key_handlers({
                "r" : self.reload,
                "1": self.switch_weapon,
                "2": self.switch_weapon,
                "3": self.switch_weapon,
                "4": self.switch_weapon,
                "5": self.switch_weapon,
                "6": self.switch_weapon,
                "7": self.switch_weapon,
                "8": self.switch_weapon,
                "9": self.switch_weapon,
                "0": self.switch_weapon,
            })
    # accept key,handler or a dict of key,handlers
    def _bind_key_handlers(self, key, handler=None):
        if handler is None:
            assert type(key) == dict
            for k, h in key.items():
                self.key_handlers[k] = h
        else:
            self.key_handlers[key] = handler

    def on_key_press(self, symbol, modifiers=None):
        self.key_handlers.get(symbol, lambda x,y: None)(symbol, modifiers)

    def reload(self, symbol, modifiers=None):
        print 'reloading'
        GameStateGlobal.agent.reload()

    def switch_weapon(self, symbol, modifiers=None):
        #print 'switch weapon'
        #print symbol, modifiers
        #print str(symbol)
        try:
            weapon_index = int(symbol)
        except (ValueError, TypeError):
            return
        print 'attempting to switch weapon to ', weapon_index
        GameStateGlobal.agent.weapons.switch(weapon_index)


from game_state import GameStateGlobal
from chat_client import ChatClientGlobal
from net_out import NetOut

inputEventGlobal = InputEventGlobal()
SDL.input.set_input_callback(inputEventGlobal)
