''' User '''
name = 'steve'
alt_name = '[%s]' % (name,)

''' Network '''
server = '127.0.0.1'
tcp_port = 5055
udp_port = 5056

''' Game Preferences '''
auto_assign_team = True

''' Window / Camera '''
width = 1280
height = 800
fullscreen = False
fov = 85.

''' Controls '''
# lower is more sensitive
sensitivity = 125.
camera_speed = 0.3
invert_mouse = False
agent_motion = 'normal'

''' HUD/Info panels '''
hud = True  # show hud
diagnostic_hud = True # show diagnostic HUD elements (graphs, ping, fps)

inventory_hud_x_offset = 250
inventory_hud_y_offset = 500

#cube_selector_x_offset = 888.0
#cube_selector_y_offset = 130.0

fps_perf_graph_x_offset = 50.0
fps_perf_graph_y_offset = 700.0

network_latency_graph_x_offset = 780.0
network_latency_graph_y_offset = 400.0

map_vbo_indicator_x_offset = 50.0
map_vbo_indicator_y_offset = 50.0

fps = True  # display FPS
ping = True # calculate and display ping
ping_update_interval = 500  # ms delay between ping updates

''' Sound '''
sound = False # global sound.
sfx = 100
music = 100

