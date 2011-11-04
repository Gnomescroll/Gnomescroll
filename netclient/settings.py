''' User '''
name = 'a_name'
alt_name = '[%s]' % (name,)

''' Rendering (will be deprecated, keep pyglet=False) '''
pyglet = False

''' Sound '''
sound = True # global sound.
sfx = 100
music = 100

''' Window '''

#xy resolution
width = 1280
height = 800
#fullscreen
fullscreen = False
widescreen = False

''' HUD '''
inventory_hud_x_offset = 250
inventory_hud_y_offset = 500

block_selector_x_offset = 888.0
block_selector_y_offset = 130.0

fps_perf_graph_x_offset = 50.0
fps_perf_graph_y_offset = 70.0

network_latency_graph_x_offset = 780.0
network_latency_graph_y_offset = 400.0

map_vbo_indicator_x_offset = 50.0
map_vbo_indicator_y_offset = 50.0

''' Controls '''
# lower is more sensitive
mouse_sensitivity = 300
camera_sensitivity = 300
sensitivity = 90 # both camera and mouse(agent) sensitivity, if those are undefined

camera_speed = 0.3


''' Information '''
fps = True  # display FPS
ping = True # calculate and display ping
ping_update_interval = 500  # ms delay between ping updates


''' Game '''
auto_assign_team = True
