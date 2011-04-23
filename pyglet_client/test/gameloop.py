from __future__ import division
from random import uniform

from pyglet import clock, font, image, window
from pyglet.gl import *

def draw(self):
    glLoadIdentity()
    glTranslatef(self.x, self.y, 0.0)
    glRotatef(self.rot, 0, 0, 1)
    glScalef(self.size, self.size, 1.0)
    glBegin(GL_TRIANGLES)
    glColor4f(1.0, 0.0, 0.0, 0.0)
    glVertex2f(0.0, 0.5)
    glColor4f(0.0, 0.0, 1.0, 1.0)
    glVertex2f(0.2, -0.5)
    glColor4f(0.0, 0.0, 1.0, 1.0)
    glVertex2f(-0.2, -0.5)
    glEnd()

def convert_index(index, height, width):
    index = int(index)
    height = int(height)
    width = int(width)
    x_ = index % width
    y_ = int((index - x_) / width)
    y = height - y_ -1
    rvalue =  x_ + y*width
    #print "rvalue= " + str(rvalue)
    return rvalue


class CubeProperties(object):

    def __init__(self):
        self.cubes = {
        0 : {
            'id' : 0,
            'occludes' : False,
            'active' : False,
            },
        1 : {
            'id' : 1,
            'occludes' : True,
            'active' : True,

            'texture' : [ #t, b, w, e, n, s
            (0, []),  #top
            (0, []),  #bottom
            (0, []), #west
            (0, []), #east
            (0, []), #north
            (0, []), #south
            ],
            },
        2 : {
            'id' : 2,
            'occludes' : True,
            'active' : True,

            'texture' : [ #t, b, w, e, n, s
            (0, [0,1,2,3]),  #top
            (6, [0,1,2,3]),  #bottom
            (7, [0,1,2,3]), #west
            (3, [0,1,2,3]), #east
            (4, [0,1,2,3]), #north
            (5, [0,1,2,3]), #south
            ],
        },
        3 : {
            'id' : 3,
            'occludes' : True,
            'active' : True,

            'texture' : [ #t, b, w, e, n, s
            (1, [0,1,2,3]),  #top
            (1, [0,1,2,3]),  #bottom
            (1, [0,1,2,3]), #west
            (1, [0,1,2,3]), #east
            (1, [0,1,2,3]), #north
            (1, [0,1,2,3]), #south
            ],
        },
     }

    def getTexture(self, tile_id, side_num):
        if self.cubes.has_key(tile_id):
            tex_a = self.cubes[tile_id]['texture']
            return tex_a[side_num]

        else:
            return 0

    def isActive(self, tile_id):
        if self.cubes.has_key(tile_id):
            return self.cubes[tile_id]['active']
        else:
            print "Error, cube type does not exist"
            return False

    def isOcclude(self, tile_id):
        if self.cubes.has_key(tile_id):
            return self.cubes[tile_id]['occludes']
        else:
            print "Error, cube type does not exist"
            return False

class CubeRenderCache(object):

    def __init__(self, cubeProperties, textureGrid):
        self.cubeProperties = cubeProperties
        self.textureGrid = textureGrid
        self.c4b_cache = {}
        self.t4f_cache = {}

        self.v_index = [
        [ 0,1,1 , 0,0,1 , 1,0,1 , 1,1,1 ], #top
        [ 1,0,0 , 0,0,0 , 0,1,0 , 1,1,0 ], #bottom
        [ 0,1,1 , 1,1,1 , 1,1,0 , 0,1,0 ], #north
        [ 0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 ], #south
        [ 0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 ], #west
        [ 1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 ], #east
    ]

        north_side = [ [0,1,1],[1,1,1],[1,1,0],[0,1,0] ]
        south_side = [[0,0,1],[0,0,0],[1,0,0],[1,0,1]]
        west_side = [[0,1,1],[0,1,0],[0,0,0],[0,0,1]]
        east_side = [[1,0,1],[1,0,0],[1,1,0],[1,1,1]]
        top_side = [[0,1,1],[0,0,1],[1,0,1],[1,1,1]]
        bottom_side = [[1,0,0],[0,0,0],[0,1,0],[1,1,0]]

    ## t, b, n, s, w, e
    def get_side(self, x, y, z, tile_id, side_num):
        ta = self.v_index[side_num]
        #v_list = (GLfloat * 12) [ta[0]+x,ta[1]+y,ta[2]+z , ta[3]+x,ta[4]+y,ta[5]+z , ta[6]+x,ta[7]+y,ta[8]+z , ya[9]+x,ta[10]+y,ta[11]+z ]
        v_list = [ta[0]+x,ta[1]+y,ta[2]+z , ta[3]+x,ta[4]+y,ta[5]+z , ta[6]+x,ta[7]+y,ta[8]+z , ta[9]+x,ta[10]+y,ta[11]+z ]
        c4B_list = self._get_c4B(tile_id, side_num)
        t4f_list = self._get_t4f(tile_id, side_num)
        return(v_list, c4B_list, t4f_list)

    def _get_c4B(self, tile_id, side_num):
        if self.c4b_cache.has_key((tile_id, side_num)):
            return self.c4b_cache[(tile_id, side_num)]
        else:
            ##compute from dict!
            #temp = (GLbyte * 4)[255, 255, 255, 255] * 4
            temp = [255, 255, 255, 255] * 4
            self.c4b_cache[(tile_id, side_num)] = temp
            return temp

    def _get_t4f(self, tile_id, side_num):
        if self.t4f_cache.has_key((tile_id, side_num)):
            return self.t4f_cache[(tile_id, side_num)]
        else:
            (texture_id, rotation) = self.cubeProperties.getTexture(tile_id, side_num)
            tex_tuple = self.textureGrid[convert_index(texture_id, 16, 16)].tex_coords
            if True:
                self.t4f_cache[(tile_id, side_num)] = list(tex_tuple)
                return list(tex_tuple)


class TerrainMap(object):

    def __init__(self):
        map = {}

    def get(x,y,z):
        pass

    def set(x,y,z,value):
        pass

class MapChunkManager(object):

    def __init__(self, terrainMap, cubeProperties):
        MapChunk.terrainMap = terrainMap
        MapChunk.cubeProperties = cubeProperties
        self.terrainMap = terrainMap
        self.cubeProperties = cubeProperties

    def set_map(x,y,z,tile_id):
        pass

import ctypes

class MapChunk(object):

    terrainMap = None
    cubeProperties = None
    cubeRenderCache = None

    x_chunk_size = 8
    y_chunk_size = 8
    z_chunk_size = 8

    def __init__(self, x_offset, y_offset):
        int_array_type = ctypes.c_int * (self.x_chunk_size * self.y_chunk_size *self.z_chunk_size) #map storage
        self.map_array = int_array_type()

        self.vertexList = None
        self.x_offset = x_offset
        self.y_offset = y_offset
        self.update = False
        self.empty = True

    def update_vertex_buffer(self):
        draw_list = []
        active_cube_number = 0
        culled_quads = 0
        for x in range(0, self.x_chunk_size):
            for y in range(0, self.y_chunk_size):
                for z in range(0, self.z_chunk_size):
                    tile_id = self.get_tile(x,y,z)
                    if self.cubeProperties.isActive(tile_id): #non-active tiles are not draw
                        active_cube_number += 1
                        for side_num in [0,1,2,3,4,5]:
                            if not self._is_occluded(x,y,z,side_num):
                                draw_list.append((x,y,z,tile_id, side_num))
                            else:
                                culled_quads += 1

        print "quads in chunk=" + str(active_cube_number*6)
        print "culled quads=" + str(culled_quads)
        print "draw_list= "
        for xa in draw_list:
            print str(xa)
        v_list = []
        c_list = []
        tex_list = []
        v_num = 0
        for (x,y,z,tile_id, side_num) in draw_list:
            rx = self.x_offset + x
            ry = self.x_offset + y
            rz = z

            (tv_list, tc_list, ttex_list) = self.cubeRenderCache.get_side(rx, ry, rz, tile_id, side_num)
            v_list += tv_list
            c_list += tc_list
            tex_list += ttex_list
            v_num += 4


        #print str(v_list)
        #print str(c_list)
        #print str(tex_list)
        #print str((len(v_list), len(c_list), len(tex_list)))

        if v_num != 0:
            self.vertexList = pyglet.graphics.vertex_list(v_num,
            ('v3f\static', v_list),
            ('c4B\static', c_list),
            ("t3f\static", tex_list)
        )
            self.empty = False
        else:
            print "v_num = 0!"
            self.empty = True
        self.update = False

    def _is_occluded(self,x,y,z,side_num):
        s_array = [(0,0,1), (0,0,-1), (0,1,0), (0,-1,0), (-1,0,0),(1,0,0)]
        temp = s_array[side_num]
        _x = temp[0] + x
        _y = temp[1] + y
        _z = temp[2] + z

        print str((x,y,z,self.get_tile(_x,_y,_z), side_num))

        if _x < 0 or _y < 0 or _z < 0:
            return False
        elif _x >= self.x_chunk_size or _y >= self.y_chunk_size or _z >= self.z_chunk_size:
            return False

        tile_id = self.get_tile(_x,_y,_z)
        #print str((x,y,z,tile_id, side_num))
        return self.cubeProperties.isOcclude(tile_id)


 #           return False
 #       if x+1 == self.s_chunk_size or y+1 == self.y_chunk_size or z+1 ==

    def get_tile(self, x,y,z):
       index = x+y*self.y_chunk_size+z*self.x_chunk_size * self.y_chunk_size
       if index < 0 or index >= self.x_chunk_size * self.y_chunk_size*self.z_chunk_size:
           print "invalid map index"
           return 0
       else:
           return self.map_array[index]

    def set_tile(self,x,y,z, value):
        index = x+y*self.y_chunk_size+z*self.x_chunk_size * self.y_chunk_size
        self.map_array[index] = c_int(value)

import random

class World(object):

    def __init__(self):
        #texture loading
        tile_image = pyglet.image.load('../texture/textures_01.png')
        tile_image_grid = pyglet.image.ImageGrid(tile_image, 16, 16)
        tile_texture_grid = pyglet.image.TextureGrid(tile_image_grid)
        self.texture_grid = tile_texture_grid

        self.cubeProperties = CubeProperties()
        self.cubeRenderCache = CubeRenderCache(self.cubeProperties, self.texture_grid) #needs texture grid
        #test
        self.terrainMap = TerrainMap()
        #MapChunkManager(terrainMap, cubeProperties)
    def tick(self):
        pass


    def test_chunk(self):
        self.mct = MapChunk(0,0)
        self.mct.cubeProperties = self.cubeProperties
        self.mct.cubeRenderCache = self.cubeRenderCache


        if True:
            for x in range(0, 8):
                for y in range(0, 8):
                    for z in range(0, 8):
                        rnd = random.randint(0,3)
                        self.mct.set_tile(x,y,z,rnd)
       # self.mct.set_tile(1,1,0,2)
        #fill with random crap
        self.mct.set_tile(0,0,0,3)
        self.mct.set_tile(1,0,0,3)
        self.mct.set_tile(2,0,0,3)
        #profile.run(self.mct.update_vertex_buffer)
        self.mct.update_vertex_buffer()

    def draw_chunk(self):
        glEnable(GL_CULL_FACE);
        glEnable(self.texture_grid.target)
        glBindTexture(self.texture_grid.target, self.texture_grid.id)

        self.mct.vertexList.draw(pyglet.gl.GL_QUADS)

    def draw(self):
        for x in range(-20, 20):
            self.draw_point(x, 0, 0, 255, 0)

        for y in range(-20, 20):
            self.draw_point(0, y, 0, 0, 255)

        self.draw_chunk()
        #self.draw_cube(0,0,0)
        #self.draw_cube2(2,0,0,1)
        #self.draw_cube2(0,2,0,2)
        #self.draw_cube2(0,0,0,2)

    def draw_point(self, x, y, r, g, b):
        z=0
        pyglet.graphics.draw(1, pyglet.gl.GL_POINTS,
            ('v3f', (x, y, z)),
            ('c3B', (r, g, b)) )

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

        t_list = [0,6,7,3,4,5]
        tc_list = []
        for texture_id in t_list:
            #tc_list = tc_list + list(self.texture_grid[0].tex_coords)
            tc_list = tc_list + list(self.texture_grid[convert_index(texture_id, 16, 16)].tex_coords)

        tile_id = 1

        glEnable(GL_CULL_FACE);
        glEnable(self.texture_grid.target) #???
        glBindTexture(self.texture_grid.target, self.texture_grid.id)
        pyglet.graphics.draw(4*6, pyglet.gl.GL_QUADS,
        ("v3f", v_list),
        ("c4B", [255, 255, 255, 255] * 4*6),
        ("t3f", tc_list))

    def draw_cube2(self, x, y, z, tile_id):
        v_list = []
        c_list = []
        tex_list = []

        v_num = 0
        for side_num in range(0, 6):
            (tv_list, tc_list, ttex_list) = self.cubeRenderCache.get_side(x, y, z, tile_id, side_num)
            (tv_list, tc_list, ttex_list) = self.cubeRenderCache.get_side(x, y, z, tile_id, side_num)
            v_list += tv_list
            c_list += tc_list
            tex_list += ttex_list
            v_num += 4

        glEnable(self.texture_grid.target) #???
        glBindTexture(self.texture_grid.target, self.texture_grid.id)
        pyglet.graphics.draw(v_num, pyglet.gl.GL_QUADS,
        ("v3f", v_list),
        ("c4B", c_list),
        ("t3f", tex_list))

from math import sin, cos, pi

class Camera(object):

    def __init__(self, win, x=0.0, y=0.0, z=0.0, rot=0.0, zoom=1.0):
        self.win = win
        self.x = x
        self.y = y
        self.z = z
        self.x_angle = 0.0
        self.y_angle = 0.0
        self.rot = rot
        self.zoom = zoom
        self.rts = True

    def worldProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        aspect = self.win.width / float(self.win.height)
        #gluPerspective( 45.0 / self.scale, aspect, 0.1, 100.0);
        gluPerspective(65, aspect, .1, 1000)


        glMatrixMode( GL_MODELVIEW )
        glLoadIdentity()

        camera_focus_x = self.x + cos( self.x_angle * pi)
        camera_focus_y = self.y + sin( self.x_angle * pi)
        camera_focus_z = self.z + sin( self.y_angle)

        gluLookAt( self.x, self.y, self.z,
                camera_focus_x, camera_focus_y, camera_focus_z,
                0., 0., 1.0)

        glEnable (GL_DEPTH_TEST)
        glEnable(GL_CULL_FACE);

    def hudProjection(self):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluOrtho2D(0, self.win.width, 0, self.win.height)

    def move_camera(self, dx, dy, dz):

        if self.rts == True:
            #dx delta
            self.x += dx*cos(self.x_angle * pi)
            self.y += dx*sin(self.x_angle * pi)
            #dy delta
            self.x += dy*cos(self.x_angle * pi + pi/2)
            self.y += dy*sin(self.x_angle * pi + pi/2)
        else:
            self.x += dx
            self.y += dy
        #dz delta
        self.z += dz

    def pan(self, dx_angle, dy_angle):
        #print "dx_angle= " + str(dx_angle)
        #print "dy_angle= " + str(dy_angle)
        self.x_angle += dx_angle
        self.y_angle += dy_angle

    def on_resize(self, width, height):
        print "Resize Window"
        glViewport(0, 0, width, height)
        self.worldProjection()
        return pyglet.event.EVENT_HANDLED

class Hud(object):

    def __init__(self, win):
        helv = font.load('Helvetica', win.width / 15.0)
        self.text = font.Text(
            helv,
            'Hello, World!',
            x=win.width / 2,
            y=win.height / 2,
            halign=font.Text.CENTER,
            valign=font.Text.CENTER,
            color=(1, 1, 1, 0.5),
        )
        self.fps = clock.ClockDisplay()

    def draw(self):
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        #self.text.draw()
        self.draw_recticle()
        self.fps.draw()

    def draw_recticle(self):
        x=win.width / 2,
        y=win.height / 2,

class Mouse(object):

    def __init__(self, main):
        self.main = main
        self.camera = main.camera

    def on_mouse_drag(self, x, y, dx, dy, buttons, modifiers):
        #print 'dy, dy = ' +  str(dx) + ' ' + str(dy)
        sen = 50
        self.camera.pan(dx*-1.0 / sen, dy*1.0 / sen)


from pyglet.window.key import symbol_string
from pyglet.window import key

class Keyboard(object):

    def __init__(self, main):
        self.main = main
        self.camera = main.camera
        self.key_handlers = {}

    def on_key_press(self, symbol, modifiers):
        v = 0.2

        if symbol == key.SPACE:
            print "Space Bar Pressed"
        #elif symbol == key.W:
            #self.camera.move_camera(v,0,0)
        #elif symbol == key.S:
            #self.camera.move_camera(-v,0,0)
        #elif symbol == key.A:
            #self.camera.move_camera(0,v,0)
        #elif symbol == key.D:
            #self.camera.move_camera(0,-v,0)
        elif symbol == key.R:
            self.camera.move_camera(0,0,v)
        elif symbol == key.F:
            self.camera.move_camera(0,0,-v)
        elif symbol in self.key_handlers:
            self.key_handlers[symbol]()

    def stateHandler(self, keyboard):
        v = 0.1
        if keyboard[key.W]:
            self.camera.move_camera(v,0,0)
        if keyboard[key.S]:
            self.camera.move_camera(-v,0,0)
        if keyboard[key.A]:
            self.camera.move_camera(0,v,0)
        if keyboard[key.D]:
            self.camera.move_camera(0,-v,0)
        if keyboard[key.R]:
            self.camera.move_camera(0,0,v)
        if keyboard[key.F]:
            self.camera.move_camera(0,0,-v)
        if keyboard[key.SPACE]:
            print "Event A.1"

import cProfile

class App(object):

    def __init__(self):

        self.world = World()
        self.win = window.Window(fullscreen=False, vsync=False)
        self.camera = Camera(self.win)
        self.camera = Camera(self.win)
        self.keyboard = Keyboard(self)
        self.mouse = Mouse(self)
        self.hud = Hud(self.win)
        #clock.set_fps_limit(60)
        #setup events
        self.keyboard.key_handlers[key.ESCAPE] = self.win.close
        self.win.on_mouse_drag = self.mouse.on_mouse_drag
        self.win.on_key_press = self.keyboard.on_key_press

        print "App init finished"

    def mainLoop(self):
        clock.set_fps_limit(60)
        keyboard = key.KeyStateHandler()
        while not self.win.has_exit:
            self.win.push_handlers(keyboard)
            self.win.dispatch_events()
            self.keyboard.stateHandler(keyboard)

            self.world.tick()
            self.win.clear() #?

            self.camera.worldProjection()
            self.world.draw()
            self.camera.hudProjection()
            self.hud.draw()

            clock.tick()
            self.win.flip()

    def mainLoop2(self):
        self.world.test_chunk()
        clock.set_fps_limit(60)
        keyboard = key.KeyStateHandler()
        while not self.win.has_exit:
            self.win.push_handlers(keyboard)
            self.win.dispatch_events()
            self.keyboard.stateHandler(keyboard)

            self.world.tick()
            self.win.clear() #?

            self.camera.worldProjection()


            self.world.draw()
            self.camera.hudProjection()
            self.hud.draw()

            clock.tick()
            self.win.flip()
            #return

app = App()
#cProfile.run('app.mainLoop2()')
app.mainLoop2()

