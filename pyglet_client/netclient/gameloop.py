

from pyglet import clock, font, image, window
from pyglet.gl import *

#import cython
#import pyximport; pyximport.install()

from player import Player
from input import Mouse, Keyboard
from camera import Camera, Hud
from pyglet.window import key

#deprecate
import world #deprecate

from net_client import NetClientGlobal
from net_out import NetOut
from net_event import NetEventGlobal
from cube_dat import CubeGlobal
from world_state import WorldStateGlobal
from client_event import ClientEventGlobal

from chat_client import Chat

#import hotshot

class App(object):

    cubeGlobal = CubeGlobal()
    worldStateGlobal = WorldStateGlobal()
    clientEventGlobal = ClientEventGlobal()
    netClientGlobal = NetClientGlobal()     #connection
    #netOut = NetOut()                       #message out
    netEventGlobal = NetEventGlobal()       #message event

    def __init__(self):
        self.clientGlobal.init()
        self.cubeGlobal.init()
        self.worldStateGlobal.init()
        self.netEventGlobal.init()
        self.netOut.init()
        #networking code
        self.player = Player() #for testing
        #self.messageHandler = MessageHandler(self.player)
        #client_api.ClientDatagramDecoder.messageHandler = self.messageHandler #handles networking callbacks

        #other
        self.world = world.World()
        self.win = window.Window(fullscreen=False, vsync=False)
        self.camera = Camera(self.win)
        self.keyboard = Keyboard(self)
        self.mouse = Mouse(self)
        self.hud = Hud(self.win)
        #setup events
        self.keyboard.key_handlers[key.ESCAPE] = self.exit
        self.win.on_mouse_drag = self.mouse.on_mouse_drag
        self.win.on_key_press = self.keyboard.on_key_press
        self.exit = False
        print "App init finished"

    def exit(self):
        self.exit = True

    def mainLoop(self):
        self.world.test_chunk()
        self.world.add_player(self.player)
        clock.set_fps_limit(60)
        keyboard = key.KeyStateHandler()
        self.win.push_handlers(keyboard)
        #self.win.push_handlers(pyglet.window.event.WindowEventLogger())

        NetClientGlobal.connect() #starts connection
        ClientGlobal.sendMessage.request_chunk_list()

        #p = hotshot.Profile("../log/client.log")
        #p.start()
        while not self.exit:
            self.win.dispatch_events()
            self.keyboard.stateHandler(keyboard)
            [d_x, d_y, d_xa, d_za, jetpack, brake] = self.player.control_state
            self.out.send_agent_control_state(self.player.id, d_x, d_y, d_xa, d_za, jetpack, brake)
            #network events
            self.connection.attempt_recv()
            self.world.tick()
            self.win.clear() #clear window and start drawing
            self.camera.worldProjection()
            self.world.draw()
            self.camera.hudProjection()
            self.hud.draw()
            clock.tick()
            self.win.flip()
        #p.stop()
        self.win.close

app = App()
app.mainLoop()
