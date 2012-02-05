#!/usr/bin/python

'''
Client input
'''
import opts
opts = opts.opts

import init_c_lib

import camera

event_names = {
    0   :   'SDL_KEYUP',
    1   :   'SDL_KEYDOWN',
}

keystate = {}
        
class InputGlobal:
    keyboard = None
    @classmethod
    def init(cls):
        cls.keyboard = Keyboard()

class Keyboard(object):

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


class InputEventGlobal:
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
            InputGlobal.keyboard.on_key_press(key, unikey)

        elif event_name == 'SDL_KEYUP':
            keystate[keycode] = 0

inputEventGlobal = InputEventGlobal()
init_c_lib.set_input_callback(inputEventGlobal)

from chat_client import ChatClientGlobal
