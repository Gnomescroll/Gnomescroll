#!/usr/bin/python

'''
Client input
'''
import opts
opts = opts.opts

import init_c_lib

from math import sin, cos, pi
from c_lib.terrain_map import toggle_t_viz_vbo_indicator_style, toggle_terrain_map_blend_mode, refresh_map_vbo, toggle_z_buffer, save_to_disk

import camera

#handles special characters
Keystring = {}
def setup_keystring():
    global Keystring
    global special_keys
    special_keys = {
    'BACKSPACE' : 8,
    'TAB': 9,
    'ENTER': 13,
    'ESC': 27,
    'SPACE': 32,
    'DELETE': 127,
    'LEFT': 276,
    'RIGHT': 275,
    'UP' : 273,
    'DOWN' : 274,
    'LSHIFT' : 304,
    'RSHIT' : 303,
    'LCTRL' : 306,
    'RCTRL' : 305,
    'QUIT'  : 9999, # custom key
    }
    for i in range(1,255):
        Keystring[i] = chr(i)
    for key,value in special_keys.items():
        Keystring[value] = key
setup_keystring()

event_names = {
    0   :   'SDL_KEYUP',
    1   :   'SDL_KEYDOWN',
}

keystate = {}

class InputEventGlobal:
    mouse = None
    keyboard = None

    def keyboard_state(self, pressed_keys):
        keyboard = []
        for keycode in pressed_keys:
            #try:
                #print keycode, chr(keycode)
            #except:
                #pass
            temp = Keystring.get(keycode, None)
            if temp != None:
                keyboard.append(temp)
        self.keyboard.stateHandler(keyboard)

    #add support for key pressed/key released
    def keyboard_text_event(self, keycode, key, event_type):
        event_name = event_names[event_type]

        try:
            unikey = unichr(keycode)
        except:
            #print 'unichr(keycode) failed. keycode=%d' % (keycode,)
            unikey = key

        #print "keycode=%d, key=%s, unicode_key=%s" % (keycode, key, unikey,)

        # set keystate map
        if event_name == 'SDL_KEYDOWN':
            keystate[keycode] = 1
            self.keyboard.on_key_press(key, unikey)

        elif event_name == 'SDL_KEYUP':
            keystate[keycode] = 0
            self.keyboard.on_key_release(key, unikey)
        
    def mouse_event(self, button,state,x,y,):
        self.mouse.on_mouse_press(x,y,button, state)

    def mouse_motion(self, x,y,dx,dy,button):
        if button != 0:
            self.mouse.on_mouse_drag(x,y,dx,dy,button)
        else:
            self.mouse.on_mouse_motion(x,y,dx,dy)

class InputGlobal:
    cube_selector = None
    keyboard = None
    mouse = None
    agentInput = None

    #help_menu = False
    #inventory = False
    #can_jump = True
    #scoreboard = False
    #map = False
    
    #input = 'camera'
    #_inputs = ('camera', 'agent')
    #camera = 'camera'
    #_cameras = ('camera', 'agent')


    @classmethod
    def init(cls, main):
        InputGlobal.mouse = Mouse(main)
        InputGlobal.keyboard = Keyboard(main)
        InputGlobal.agentInput = AgentInput()
        cls.cube_selector = CubeSelector(8,8)

        InputEventGlobal.mouse = cls.mouse
        InputEventGlobal.keyboard = cls.keyboard

class Mouse(object):

    def __init__(self, main):
        self.main = main
        
    def on_mouse_drag(self, x, y, dx, dy, buttons, modifiers=None):
        pass

    def on_mouse_motion(self, x, y, dx, dy):
        pass
        
    #buttons:
    #1 left, 2 right, 4 scroll up, 5 scroll down
    #state is 0 or 1, 1 if mouse was click, 0 if it was released
    def on_mouse_press(self, x, y, button, state):
        pass
        #if button == 3 and state == 1:  # right click down
            #if init_c_lib.cy_input_state.input_mode == 1:
                #self.main.camera.toggle_zoom()
            #elif GameStateGlobal.agent.can_zoom():
                #self.main.agent_camera.toggle_zoom()


class Keyboard(object):

    def __init__(self, main):
        self.main = main
        self.camera = main.camera
        self.agent_camera = main.agent_camera
        self.key_handlers = {}

        self._init_key_handlers()

    def _input_callback(self, callback):
        if callable(callback):
            callback(self)

    # continuous non-character key detection
    #e.g. back space, cursor movement
    def on_text_motion(self, motion):
        if init_c_lib.cy_input_state.chat:
            callback = ChatClientGlobal.chatClient.input.on_text_motion(motion)
            self._input_callback(callback)

    # one-time non character key detection
    # e.g. enter
    def on_key_press(self, symbol, unicode_key):

        if init_c_lib.cy_input_state.chat:
            if unicode_key:
                callback = ChatClientGlobal.chatClient.input.on_key_press(symbol, unicode_key)
                self._input_callback(callback)
        else:
            if init_c_lib.cy_input_state.input_mode == 0:
                InputGlobal.agentInput.on_key_press(symbol)

            self.key_press_handlers.get(symbol, lambda : None)()

    def on_key_release(self, symbol, unicode_key):
        if init_c_lib.cy_input_state.input_mode == 0:
            InputGlobal.agentInput.on_key_release(symbol)
        self.key_release_handlers.get(symbol, lambda: None)()

    def _init_key_handlers(self):
        self.key_press_handlers = {
            "n" : toggle_t_viz_vbo_indicator_style,
            "o" : toggle_terrain_map_blend_mode,
            "l" : refresh_map_vbo,
            "v" : toggle_z_buffer,
            "p" : init_c_lib._toggle_latency_unit,
        }

        self.key_release_handlers = {
        }

    def stateHandler(self, keyboard):
        if init_c_lib.cy_input_state.chat:
            return
        if init_c_lib.cy_input_state.input_mode == 1:
            self.camera_input_mode(keyboard)
        elif init_c_lib.cy_input_state.input_mode == 0:
            self.agent_input_mode(keyboard)

    def agent_input_mode(self, keyboard):
        if GameStateGlobal.agent.dead:
            return

        f,b,l,r, jump, jet, crouch, boost, misc1, misc2, misc3 = [0]*11

        if 'w' in keyboard:
            f = 1
        if 's' in keyboard:
            b = 1
        if 'a' in keyboard:
            l = 1
        if 'd' in keyboard:
            r=1
        if 'c' in keyboard:
            boost = 1
        if 'z' in keyboard:
            jet = 1
        if 'SPACE' in keyboard and init_c_lib.cy_input_state.can_jump:
            jump = 1
            init_c_lib.cy_input_state.can_jump = False
        if 'LCTRL' in keyboard:
            crouch = 1
        #misc1=misc2=misc3=boost=0

        button_state = [f,b,l,r, jump, jet, crouch, boost, misc1, misc2, misc3]
        GameStateGlobal.agent.button_state = button_state

    def camera_input_mode(self, keyboard):
        v = opts.camera_speed

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


# only calls method if GameStateGlobal.agent is not None
def requireAgent(f):
    def requireAgent_wrap(*args, **kwargs):
        if GameStateGlobal.agent is not None:
            f(*args, **kwargs)
    return requireAgent_wrap

class AgentInput:

    def __init__(self):
        self.key_press_handlers = {}
        self.key_release_handlers = {}
        self._init_key_handlers()

    def _init_key_handlers(self):
        self.key_press_handlers = {
            'left':self.adjust_block,
            'right':self.adjust_block,
            'up':self.adjust_block,
            'down':self.adjust_block,
        }

        self.key_release_handlers = {
        }

    def on_key_press(self, symbol, modifiers=None):
        self.key_press_handlers.get(symbol, lambda s: None)(symbol)

    def on_key_release(self, symbol):
        self.key_release_handlers.get(symbol, lambda s: None)(symbol)

    @classmethod
    @requireAgent
    def adjust_block(cls, symbol=None, modifiers=None):
        if GameStateGlobal.agent.active_weapon != 2: # block applier
            return
        if symbol == 'left':
            InputGlobal.cube_selector.left()
        elif symbol == 'right':
            InputGlobal.cube_selector.right()
        elif symbol == 'up':
            InputGlobal.cube_selector.up()
        elif symbol == 'down':
            InputGlobal.cube_selector.down()


class CubeSelector(object):

    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.n = x*y
        self.active = 0

    def __setattr__(self, k, v):
        self.__dict__[k] = v
        if k == 'active':
            init_c_lib.HudCubeSelector.set_active_pos(v)
            #if GameStateGlobal.agent is not None:
                #GameStateGlobal.agent.set_active_block(self.active_id)
        elif k == 'active_id':
            init_c_lib.HudCubeSelector.set_active_id(v)

    def __getattribute__(self, k):
        if k == 'active_id':
            return init_c_lib.HudCubeSelector.get_active_id()
        if k == 'active':
            return init_c_lib.HudCubeSelector.get_active_pos()
        return object.__getattribute__(self, k)

    def vertical(self, up=True):
        shift = -1 if up else 1
        row = self.active // self.x
        col = self.active % self.x

        row = (row + shift) % self.y
        new = (row * self.x) + col

        if new < 0 or new > self.n - 1:
            return
        self.active = new

    def up(self):
        self.vertical(up=True)

    def down(self):
        self.vertical(up=False)

    def horizontal(self, left=True):
        shift = -1 if left else 1
        row = self.active // self.x
        col = self.active % self.x
        new = (col + shift) % self.x
        new += row * self.x
        if new < 0 or new > self.n - 1:
            return
        self.active = new

    def left(self):
        self.horizontal(left=True)

    def right(self):
        self.horizontal(left=False)

from game_state import GameStateGlobal
from chat_client import ChatClientGlobal
from net_out import NetOut

inputEventGlobal = InputEventGlobal()
init_c_lib.set_input_callback(inputEventGlobal)
