class Mouse(object):

    def __init__(self, main):
        self.main = main
        self.camera = main.camera
        self.player = main.player

    def on_mouse_drag(self, x, y, dx, dy, buttons, modifiers):
        #print 'dy, dy = ' +  str(dx) + ' ' + str(dy)
        sen = 50
        if buttons == 4:
            self.player.pan(dx*-1.0 / sen, dy*1.0 / sen)
        else:
            self.camera.pan(dx*-1.0 / sen, dy*1.0 / sen)


from pyglet.window.key import symbol_string
from pyglet.window import key

class Keyboard(object):

    def __init__(self, main):
        self.main = main
        self.camera = main.camera
        self.key_handlers = {}

    def on_key_press(self, symbol, modifiers):
        v = 0.2
        #print "key press"
        if symbol == key.E:
            print "E"
            self.main.world.toggle_mipmap()
        #elif symbol == key.W:
            #self.camera.move_camera(v,0,0)
        #elif symbol == key.S:
            #self.camera.move_camera(-v,0,0)
        #elif symbol == key.A:
            #self.camera.move_camera(0,v,0)
        #elif symbol == key.D:
            #self.camera.move_camera(0,-v,0)
        #elif symbol == key.R:
        #    self.camera.move_camera(0,0,v)
        #elif symbol == key.F:
        #    self.camera.move_camera(0,0,-v)
        if False:
            pass
        elif symbol in self.key_handlers:
            self.key_handlers[symbol]()

    def stateHandler(self, keyboard):
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
