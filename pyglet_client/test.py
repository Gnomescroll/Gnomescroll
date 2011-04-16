import pyglet
import math

# Disable error checking for increased performance
##pyglet.options['debug_gl'] = False

#from pyglet.gl import *

platform = pyglet.window.get_platform()
display = platform.get_default_display()
screen = display.get_default_screen()

template = pyglet.gl.Config(alpha_size=8)
config = screen.get_best_config(template)
context = config.create_context(None)
window = pyglet.window.Window(context=context)

from pyglet.gl import *

##load textures

## Saving screenshot
# pyglet.image.get_buffer_manager().get_color_buffer().save('screenshot.png')

tile_image = pyglet.image.load('./texture/textures_01.png')
tile_image_grid = pyglet.image.ImageGrid(tile_image, 16, 16)
tile_texture_grid = pyglet.image.TextureGrid(tile_image_grid)


# glBindTexture(texture.target, texture.id)

vertices = [
    0, 0,
    window.width, 0,
    window.width, window.height]
vertices_gl = (GLfloat * len(vertices))(*vertices)

width = 16
height = 16

	
def convert_index(index, height, width):
	x_ = index % width
	y_ = (index - x_) / width
	y = height - y_ -1
	return x_ + y*width

#glEnableClientState(GL_VERTEX_ARRAY)
#glVertexPointer(2, GL_FLOAT, 0, vertices_gl)

#t = texture.tex_coords
#w, h = texture.width, texture.height
#array = (GLfloat * 32)(
	#t[0][0], t[0][1], t[0][2], 1.,
     #x,       y,       z,       1.,
     #t[1][0], t[1][1], t[1][2], 1.,
     #x + w,   y,       z,       1.,
     #t[2][0], t[2][1], t[2][2], 1.,
     #x + w,   y + h,   z,       1.,
     #t[3][0], t[3][1], t[3][2], 1.,
     #x,       y + h,   z,       1.)
  
label = pyglet.text.Label('FPS: %f' % pyglet.clock.get_fps(),
                          font_name='Times New Roman',
                          font_size=12,
                          x= 10  , y=window.height, anchor_x='left', anchor_y='top')

def draw_plane(tile_id, vertices):
	#assert len(vertices) == 12
	array = (GLfloat * 32)(
	0., 0., 0., 
    0., 1. , 0.,
    1., 1 , 0.,
    1., 0. ,0. )
 
	xa = (
	0., 0., 0., 
    100., 0. , 0.,
    100., 100 , 0.,
    0., 100. ,0. )
    
	xb = (
	0., 0., 0., 
    0., 0. , 100.,
    0., 100 , 100.,
    0., 100. ,0. )
    
	pyglet.graphics.draw(4, pyglet.gl.GL_QUADS,
    ("v3f", xb),
    ("c4B", [255, 255, 255, 255] * 4),
    ("t3f", tile_texture_grid[convert_index(tile_id, 16, 16)].tex_coords )
    #("t2f", (0., 0., 0., 1., 1., 1., 1., 0.) )
)

glEnable(tile_texture_grid.target) 

gluPerspective( 45.0, 640.0 / 480.0, 0.1, 100.0);

c = 0
n = 1


camera_x = 0
camera_y = 0
camera_z = 0

camera_x_angle = 0
camera_y_angle = 0

@window.event
def on_mouse_drag(x, y, dx, dy, buttons, modifiers):
	global camera_x_angle, camera_y_angle
	print 'dy, dy = ' +  str(dx) + ' ' + str(dy)
	camera_x_angle = camera_x_angle + dx/100
	camera_z_angle = camera_y_angle + dx/100
	
@window.event
def on_draw():
	global c, n
	global camera_x, camera_y, camera_z, camera_x_angle, camera_y_angle
	c = c + 1
	if c % 2 == 0:
		c = 0 
		n = (n+1) % 255
	
	#tile_texture_grid[convert_index(n, 16, 16)].blit(30, 30, z=0, width=None, height=None)
	window.clear()
	glClear(GL_COLOR_BUFFER_BIT)
	glMatrixMode( GL_MODELVIEW ) #needed?
	
	glLoadIdentity()
	
	
	camera_focus_x = camera_x + math.cos( camera_x_angle * math.pi)
	camera_focus_y = camera_y + math.sin( camera_x_angle * math.pi)
	camera_focus_z = camera_z
    
	gluLookAt(camera_x, camera_y, camera_z,
               camera_focus_x, camera_focus_y, camera_focus_z,
               0, 0, 1) #z is up
               
	#glEnable(tile_texture_grid.target)        # typically target is GL_TEXTURE_2D
	glBindTexture(tile_texture_grid.target, tile_texture_grid.id)
	#glDrawArrays(GL_TRIANGLES, 0, len(vertices) // 2)
    #pyglet.graphics.draw_indexed(2, pyglet.gl.GL_POINTS, [0, 1, 2, 3], ('v2i', (10, 15, 30, 35)))
	draw_plane(n, None)

 #   pyglet.graphics.draw(2, pyglet.gl.GL_POINTS,
 #   ('v2i', (10, 15, 30, 35)),
#    ('c3B', (0, 0, 255, 0, 255, 0))
#
	#x = 0.
	#y = 0.
	#z = 0.

	#glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT)
	#glInterleavedArrays(GL_T4F_V4F, 0, array)
	#glDrawArrays(GL_QUADS, 0, 4)
	#glPopClientAttrib()
	label.text = 'FPS: %f' % pyglet.clock.get_fps()
	label.draw()
	#dt = pyglet.clock.tick()
    # ... update and render ...
	#print 'FPS is %f' % pyglet.clock.get_fps()
    
pyglet.app.run()
    
def draw_tile(tile_id, x, y, z):
	pass

#need 4 vertices, clockwise
