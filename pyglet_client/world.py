
import pyglet
from pyglet.gl import *

def convert_index(index, height, width):
	x_ = index % width
	y_ = (index - x_) / width
	y = height - y_ -1
	return x_ + y*width


class World:
	
	def __init__(self, camera):
		self.camera = camera
		
		tile_image = pyglet.image.load('./texture/textures_01.png')
		tile_image_grid = pyglet.image.ImageGrid(tile_image, 16, 16)
		tile_texture_grid = pyglet.image.TextureGrid(tile_image_grid)
		
		self.texture_grid = tile_texture_grid

	#process updates
	def tick(self):
		pass

	def _quad_vertices(self, list):
		x = []
		for v in list:
			for c in v:
				x.append(c)
		return x

	def draw_cube(self, x,y,z):
		vertex = [
		(0,0,0), (0,1,0), (1,1,0), (1,0,0), 
		(0,0,1), (0,1,1), (1,1,1), (1,0,1) ]
	
		#by default, counterclockwise polygons are frontfacing
		north_side = [ [0,1,1],[1,1,1],[1,1,0],[0,1,0] ]
		south_side = [[0,0,1],[0,0,0],[1,0,0],[1,0,1]]
		west_side = [[0,1,1],[0,1,0],[0,0,0],[0,0,1]]
		east_side = [[1,0,1],[1,0,0],[1,1,0],[1,1,1]]
		top_side = [[0,1,1],[0,0,1],[1,0,1],[1,1,1]]
		bottom_side = [[1,0,0],[0,0,0],[0,1,0],[1,1,0]]
		
		quad_list = [north_side, south_side, west_side, east_side, top_side, bottom_side]
		v_list = []
		for quad in quad_list:
			v_list = v_list + self._quad_vertices(quad)
		
		t_list = [0,0,0,0,0,0]
		tc_list = []
		for texture_id in t_list:
			tc_list = tc_list + list(self.texture_grid[convert_index(texture_id, 16, 16)].tex_coords)
		
		tile_id = 1
		
		#print "len_v= " + str(len(v_list))
		#glMatrixMode(GL_MODELVIEW)
		#glLoadIdentity() 
				
		glEnable(self.texture_grid.target) #???
		glBindTexture(self.texture_grid.target, self.texture_grid.id)
		pyglet.graphics.draw(4*6, pyglet.gl.GL_QUADS,
	    ("v3f", v_list),
	    ("c4B", [255, 255, 255, 255] * 4*6),
	    ("t3f", tc_list))
	    #("t2f", (0., 0., 0., 1., 1., 1., 1., 0.) )
	
		#(GLfloat * 32)(
	#t[0][0], t[0][1], t[0][2], 1.,
     #x,       y,       z,       1.,
     #t[1][0], t[1][1], t[1][2], 1.,
     #x + w,   y,       z,       1.,
     #t[2][0], t[2][1], t[2][2], 1.,
     #x + w,   y + h,   z,       1.,
     #t[3][0], t[3][1], t[3][2], 1.,
     #x,       y + h,   z,       1.)
	
	def draw_plane(self, texture_id):
		#glEnable(self.texture_grid.target) 

		x= 0.
		y = 0.
		z = 0.
		
		xb = (
		0.+x, 0.+y, 0.+z, 
	    1.+x, 0.+y , 0.+z,
	    1.+x, 1.+y , 0.+z,
	    1.+x, 1.+y ,0.+z)
		
		glEnable(self.texture_grid.target) #???
		glBindTexture(self.texture_grid.target, self.texture_grid.id)
			    
		pyglet.graphics.draw(4, pyglet.gl.GL_QUADS,
	    ("v3f", xb),
	    ("c4B", [255, 255, 255, 255] * 4),
	    ("t3f", self.texture_grid[convert_index(texture_id, 16, 16)].tex_coords )
	    #("t2f", (0., 0., 0., 1., 1., 1., 1., 0.) )
	)
	
	def draw_point(self, x, y, r, g, b):
		z=0
		pyglet.graphics.draw(1, pyglet.gl.GL_POINTS,
			('v3f', (x, y, z)),
			('c3B', (r, g, b)) )

	def draw(self, window):
		
		size = 600
		
		glClear(GL_COLOR_BUFFER_BIT)
		glBegin(GL_TRIANGLES)
		glVertex3f(0, 0, 0)
		glVertex3f(size, 0, 0)
		glVertex3f(size, size, 0)
		glEnd()
		
		self.draw_plane(0)
		
		for x in range(-20, 20):
			self.draw_point(x, 0, 0, 255, 0)
			
		for y in range(-20, 20):
			self.draw_point(0, y, 0, 0, 255)
		#self.draw_cube(0,0,0)
		#glEnable(GL_CULL_FACE);
		#glFrontFace()
