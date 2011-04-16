import pyglet

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

tile_image = pyglet.image.load('textures_01.png')
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
                          
@window.event
def on_draw():
	window.clear()
	n = 0
	tile_texture_grid[convert_index(n, 16, 16)].blit(30, 30, z=0, width=None, height=None)
	#glClear(GL_COLOR_BUFFER_BIT)
	#glLoadIdentity()
	#glDrawArrays(GL_TRIANGLES, 0, len(vertices) // 2)
    #pyglet.graphics.draw_indexed(2, pyglet.gl.GL_POINTS, [0, 1, 2, 3], ('v2i', (10, 15, 30, 35)))
 #   pyglet.graphics.draw(2, pyglet.gl.GL_POINTS,
 #   ('v2i', (10, 15, 30, 35)),
#    ('c3B', (0, 0, 255, 0, 255, 0))
#)
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
