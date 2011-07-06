ctypedef unsigned char Uint8

ctypedef struct MouseMotion:
    int x
    int y
    int dx
    int dy
    int button

ctypedef struct MouseEvent:
    int x
    int y
    int button
    int state

### call backs
cdef extern from "input_functions.h":

    ctypedef int (*key_state_func)(Uint8* keystate, int numkeys)
    int _key_state_callback(key_state_func user_func, Uint8* keystate, int numkeys)

    #deprecate
    ctypedef int (*key_event_func)(char key)
    int _key_event_callback(key_event_func user_func, char key)

    ctypedef int (*mouse_motion_func)(MouseMotion ms)
    int _mouse_motion_callback(mouse_motion_func user_func, MouseMotion)

    ctypedef int (*mouse_event_func)(MouseEvent me)
    int _mouse_event_callback(mouse_event_func user_func, MouseEvent me)

    ctypedef int (*key_text_event_func)(char key, char* key_name)
    int _key_text_event(key_text_event_func user_func, char key, char* key_name)

    int _init_input()
#    int _get_key_state(key_state_func key_state_cb)
#    int _process_events(mouse_event_func mouse_event_cb, mouse_motion_func mouse_motion_cb, key_event_func keyboard_event_cb, key_text_event_func keyboard_text_event_cb)
#    int _set_text_entry_mode(int n)

## input.c

#cdef extern int _init_input()
cdef extern int _get_key_state(key_state_func key_state_cb)
cdef extern int _process_events(mouse_event_func mouse_event_cb, mouse_motion_func mouse_motion_cb, key_event_func keyboard_event_cb, key_text_event_func keyboard_text_event_cb)
cdef extern int _set_text_entry_mode(int n)

def get_key_state():
    _get_key_state(&key_state_callback)

def process_events():
    temp = _process_events(&mouse_event_callback, &mouse_motion_callback, &key_event_callback, &key_text_event_callback)
    #mouse_event_func mouse_event_cb, mouse_motion_func mouse_motion_cb, key_event_func keyboard_event_cb)

def set_text_entry_mode(int n):
    temp = _set_text_entry_mode(n)

#cpdef int call_back_test():
#    _key_event_callback(&key_event_callback, 42)

#import input

class Callback_dummy:
    def keyboard_state(self, pressed_keys):
        pass
    def keyboard_event(self, key):
        pass
    def keyboard_text_event(self, key, key_string):
        pass
    def mouse_event(self, button, state, x, y):
        pass
    def mouse_motion(self, x,y,dx,dy,button):
        pass

input_callback = Callback_dummy()

#input_callback = None

def set_input_callback(callback):
    global input_callback
    input_callback = callback
    print "Input Callback Set"

cdef int key_state_callback(Uint8* keystate, int numkeys):
    global input_callback
    pressed_keys = []
    for i in range(0, numkeys):
        if keystate[i] != 0:
            pressed_keys.append(i)
    #input.inputEventGlobal.keyboard_state(pressed_keys)
    input_callback.keyboard_state(pressed_keys)

cdef int key_event_callback(char key):
    #print "key event___"
    global input_callback
    input_callback.keyboard_event(key)

cdef int key_text_event_callback(char key, char* key_name):
    global input_callback
    #print "keycode_="+ str(key)
    cdef bytes py_string
    py_string = key_name
    key_string = key_name.decode('ascii')
    #input.inputEventGlobal.keyboard_text_event(key, key_string)
    input_callback.keyboard_text_event(key, key_string)

cdef int mouse_motion_callback(MouseMotion ms):
    global input_callback
    input_callback.mouse_motion(ms.x,ms.y, ms.dx,-1*ms.dy, ms.button)
    #input.inputEventGlobal.mouse_motion(ms.x,ms.y,ms.dx,ms.dy, ms.button)

cdef int mouse_event_callback(MouseEvent me):
    global input_callback
    input_callback.mouse_event(me.button, me.state, me.x, -1*me.y)
    #input.inputEventGlobal.mouse_event(me.button, me.state, me.x, me.y)

def init():
    _init_input()
