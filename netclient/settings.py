pyglet = False

''' User '''
name = 'a_name'
alt_name = '[%s]' % (name,)

''' Rendering (will be deprecated, keep pyglet=False) '''
pyglet = False
#graphics = "SDL" # options, "SDL", "pyglet"

''' Sound '''
audio = True
sfx = 100
music = 100

''' Window '''
_s_mode = 2
if _s_mode == 0:	#3 to 4
	width  = 1024
	height = 800
if _s_mode == 1:	#full wide screen
	width  = 1920
	height = 1200
if _s_mode == 2:	#windows wide screen
	width = 1280
	height = 800
fullscreen = False

''' Controls '''
# lower is more sensitive
mouse_sensitivity = 300
camera_sensitivity = 300
sensitivity = 90 # both camera and mouse(agent) sensitivity, if those are undefined

''' Information '''
fps = True  # display FPS
ping = True # calculate and display ping
ping_update_interval = 500  # ms delay between ping updates


''' Game '''
auto_assign_team = True

try:
    from settings import *
except ImportError:
    print 'settings.py file not found, using all defaults'
