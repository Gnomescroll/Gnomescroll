import pyglet

win = pyglet.window.Window(fullscreen= False)
text = pyglet.font.Text(pyglet.font.load ('Arial', 16), 'Hello, World!')

text.x = 60 #x offset
text.y = 60 #y offset

'''
#this sets string
text.str = "test"
'''


'''
drawing images:
img = pyglet.image.load ('test.png')
img.blit (50, 50)
'''

while not win.has_exit:

    win.dispatch_events() #gets keyboard events
    win.clear()           #clears screen
    text.draw()           #draw text
    win.flip()            #blip


# from pyglet.gl import *

'''
draw points in 3d space with OpenGl

pyglet.graphics.draw(1, pyglet.gl.GL_POINTS,
        ('v3f', (x, y, z)),
        ('c3B', (r, g, b)) )
'''

'''
drawing lines

        pyglet.graphics.draw(6, GL_LINES,
        ("v3f", [x1,y1,z1] + [x2,y2,z2]),
        ("c3B", [255, 255, 0] *2)
        )
'''

'''
drawing quads

v_list = [] #choose 12 numbers, 3 floats for each vertex
c = [255,255,0] #color you want the quad


pyglet.graphics.draw(12, gl.GL_QUAD,
("v3f", v_list,
("c3B", c*4)
)

'''



'''
setting camera position for 3d viewing

#point you want to look from
look_from_x = 0
look_from_y = 0
look_from_z = 1
#point you want to look at
look_at_x = 0
look_at_y = 0
look_at_z = 0

glMatrixMode(GL_PROJECTION)
glLoadIdentity()
aspect = win.width / float(self.win.height)
gluPerspective(65, aspect, .1, 1000)  #show objects from .1 to 1000 distance

glMatrixMode( GL_MODELVIEW )
glLoadIdentity()

gluLookAt( look_from_x, look_from_y, look_from_z
        look_at_x, look_at_y, look_at_z
        0., 0., 1.0)

glEnable (GL_DEPTH_TEST)
'''
