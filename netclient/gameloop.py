#!/c/Python27/python.exe

# boot hack
import sys
import os
print "Working Directory: %s" % (os.getcwd())
sys.path.insert(0, './ext/')
sys.path.insert(0, '/c/dc_mmo/netclient/ext/')
sys.path.insert(0, 'c:/dc_mmo/netclient/ext/')

# load arguments & settings
import args_client
import opts
opts.opts = args_client.get_args()
opts = opts.opts

import c_lib.terrain_map
import init_c_lib

from init_c_lib import START_CLOCK
from init_c_lib import NetClientConnectTo
from dat_loader import dat_loader

init_c_lib.set_resolution(opts.width, opts.height, fullscreen=opts.fullscreen)

class App(object):

    def init_sound(self):
        path = './media/sound/wav/'
        soundfiles = os.listdir(path)
        init_c_lib.Sound.init(path, soundfiles, enabled=opts.sound, sfxvol=opts.sfx, musicvol=opts.music)

    def __init__(self):
        init_c_lib.load_options(opts)
        init_c_lib.init()

        def load_cube_dat():
            import cube_dat
            dat_loader.load('cubes', cube_dat.dat)
        load_cube_dat()

        c_lib.terrain_map.init()
        init_c_lib.Font.init()
        self.init_sound()
        init_c_lib.choose_name(opts.name)
        
        print "Client init finished"

    def connect(self):
        START_CLOCK() #clock must be started before networking stuff

        try:
            a,b,c,d = map(int, opts.server.split("."))
            print "Ip address: %s" % (opts.server,)
        except ValueError, e:
            print e
            print "Invalid server ip address %s" % (opts.server,)
            sys.exit(1)
        NetClientConnectTo(a,b,c,d, opts.port)

    def mainLoop(self):
        self.connect()
        init_c_lib.run_game()
        init_c_lib.close()

if __name__ == '__main__':
    app = App()
    app.mainLoop()
else:
    print "Not Run as Main!"

sys.exit()
