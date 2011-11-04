'''
Default settings for client

DO NOT MODIFY

Edit your settings in settings.py
'''

''' User '''
name = 'a_name'
alt_name = '[%s]' % (name,)

''' Rendering (will be deprecated, keep pyglet=False) '''
pyglet = False

''' Sound '''
sound = True
sfx = 100
music = 100

''' Window '''
width = 1024
height = 800
fullscreen = False

''' Controls '''
# lower is more sensitive
mouse_sensitivity = 300
camera_sensitivity = 300
sensitivity = 90 # both camera and mouse(agent) sensitivity, if those are undefined

camera_speed = 0.6

''' HUD/Information '''
fps = True  # display FPS
ping = True # calculate and display ping
ping_update_interval = 500  # ms delay between ping updates


''' Game '''
auto_assign_team = True

''' DEBUG '''
draw_agents = True

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
