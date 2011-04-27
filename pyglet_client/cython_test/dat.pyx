import pyglet
from pyglet.gl import *

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

#from ctypes import *
import ctypes

class MapChunk(object):

    terrainMap = None
    cubeProperties = None
    cubeRenderCache = None

    x_chunk_size = 8
    y_chunk_size = 8
    z_chunk_size = 64

    def __init__(self, x_offset, y_offset):
        int_array_type = ctypes.c_int * (self.x_chunk_size * self.y_chunk_size *self.z_chunk_size) #map storage
        self.map_array = int_array_type()

        self.vertexList = None
        self.x_offset = x_offset
        self.y_offset = y_offset
        self.update = True
        self.empty = True

    def update_vertex_buffer(self, batch = None):
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

        if False:
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
            ry = self.y_offset + y
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

        if v_num == 0:
            self.empty = True
            self.update = False
            return

        if batch == None:
            self.vertexList = pyglet.graphics.vertex_list(v_num,
            ('v3f\static', v_list),
            ('c4B\static', c_list),
            ("t3f\static", tex_list)
        )
        else:
            self.vertexList = batch.add(v_num, pyglet.gl.GL_QUADS, None,
            ('v3f\static', v_list),
            ('c4B\static', c_list),
            ("t3f\static", tex_list)
            )

        self.update = False

    def _is_occluded(self,x,y,z,side_num):
        s_array = [(0,0,1), (0,0,-1), (0,1,0), (0,-1,0), (-1,0,0),(1,0,0)]
        temp = s_array[side_num]
        _x = temp[0] + x
        _y = temp[1] + y
        _z = temp[2] + z

        #print str((x,y,z,self.get_tile(_x,_y,_z), side_num))

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
        self.map_array[index] = ctypes.c_int(value)
