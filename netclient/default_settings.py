'''
Default settings for server

DO NOT MODIFY

Edit your settings in settings.py
'''

''' User '''
name = 'a_name'
alt_name = '[%s]' % (name,)

''' Rendering (will be deprecated, keep pyglet=False) '''
pyglet = False
#graphics = "SDL" # options, "SDL", "pyglet"

''' Sound '''
audio = False
sfx = 100
music = 100

''' Window '''
width = 1024
height = 800
fullscreen = 0

''' Controls '''
# lower is more sensitive
mouse_sensitivity = 300
camera_sensitivity = 300
sensitivity = 90 # both camera and mouse(agent) sensitivity, if those are undefined

''' HUD/Information '''
fps = True  # display FPS
ping = True # calculate and display ping
ping_update_interval = 500  # ms delay between ping updates


''' Game '''
auto_assign_team = True

try:
    from settings import *
except ImportError:
    print 'settings.py file not found, using all defaults'
