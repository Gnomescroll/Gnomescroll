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
from game_state import GameStateGlobal

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
        
class InputGlobal:
    keyboard = None
    @classmethod
    def init(cls, main):
        InputGlobal.keyboard = Keyboard(main)
        InputEventGlobal.keyboard = cls.keyboard

class Keyboard(object):

    def __init__(self, main):
        self.main = main
        self.camera = main.camera
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
            self.key_press_handlers.get(symbol, lambda : None)()

    def on_key_release(self, symbol, unicode_key):
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

class InputEventGlobal:
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

inputEventGlobal = InputEventGlobal()
init_c_lib.set_input_callback(inputEventGlobal)

from chat_client import ChatClientGlobal
