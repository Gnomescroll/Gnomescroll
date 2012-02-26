'''
Default settings for client

DO NOT MODIFY

Edit your settings in settings.py
'''

''' User '''
name = 'your_name'
alt_name = '[%s]' % (name,)

''' Network '''
server = '174.37.26.119'  # use internal default
port = 0            # use internal default

''' Game Preferences '''
auto_assign_team = True

''' Window / Camera '''
width = 1024
height = 800
fullscreen = False
fov = 85.

''' Controls '''
sensitivity = 1000
camera_speed = 0.6
invert_mouse = False
agent_motion = 'normal'

''' HUD/Information '''
hud = True  # show hud
diagnostic_hud = True # show diagnostic HUD elements (graphs, ping, fps)

inventory_hud_x_offset = 250
inventory_hud_y_offset = 500

cube_selector_x_offset = 150
cube_selector_y_offset = 150

fps_perf_graph_x_offset = 50.0
fps_perf_graph_y_offset = 70.0

network_latency_graph_x_offset = 780.0
network_latency_graph_y_offset = 400.0

map_vbo_indicator_x_offset = 50.0
map_vbo_indicator_y_offset = 50.0

fps = True  # display FPS
ping = True # calculate and display ping
ping_update_interval = 500  # ms delay between ping updates

font = "inconsolata_18.fnt"
font_size = 18

''' Sound '''
sound = True
sfx = 100
music = 100

#######
import os.path
if os.path.exists("./settings.py"):
    print "Loading Settings File"
    execfile("./settings.py")
else:
    try:
        from settings import *
    except ImportError:
        print 'settings.py file not found, using all defaults'

print "Window resolution - %dx%d" % (width, height,)
