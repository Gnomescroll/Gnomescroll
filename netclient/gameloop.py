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

import init_c_lib

from dat_loader import dat_loader

class App(object):

    def __init__(self):
        if opts.sound_devices:
            init_c_lib.show_sound_devices()
            sys.exit()
        init_c_lib.load_options(opts)
        init_c_lib.init_game()

        #def load_cube_dat():
        #    import cube_dat
        #    dat_loader.load('cubes', cube_dat.dat)
        
        #load_cube_dat()
        #init_c_lib.init_terrain()
        
        #init_c_lib.Font.init()
        #init_c_lib.start_font()
        
        print "Client init finished"

    def run(self):
        init_c_lib.run_game()

if __name__ == '__main__':
    App().run()
else:
    print "gameloop.py not run as __main__!"

sys.exit()
