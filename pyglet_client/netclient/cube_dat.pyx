import pyglet
from pyglet.gl import *


from terrain_map import TerrainMap

cube_list = {
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
    4 : {
        'id' : 4,
        'occludes' : True,
        'active' : True,

        'texture' : [ #t, b, w, e, n, s
        (16, [0,1,2,3]),  #top
        (16, [0,1,2,3]),  #bottom
        (16, [0,1,2,3]), #west
        (16, [0,1,2,3]), #east
        (16, [0,1,2,3]), #north
        (16, [0,1,2,3]), #south
        ],
    },
 }

#cdef class CubePhysical:
    #cdef int id = 0
    #cdef int active = 0
    #cdef int occludes = 0

    #def __init__(CubePhysical self, int id,int active,int occludes):
        #self.id = id
        #self.active = active
        #self.occludes = occludes

#physical cube properties
cdef struct CubePhysical:
    int id
    int active
    int occludes

#used for initing the struct
cdef void init_CubePhysical(CubePhysical*x, int id, int active, int occludes):
    x.id = id
    x.active = active
    x.occludes = occludes

cdef enum:
    max_cubes = 4096

cdef class CubePhysicalProperties:
    cdef CubePhysical cube_array[4096]

    def __init__(self):
        global cube_list
        for cube in cube_list.values():
            self.add_cube(cube)

    def add_cube(self, d):
        id = int(d['id'])
        if id >= max_cubes: #max number of cubes
            print "Error: cube id is too high"
            return
        active = int(d['active'])
        occludes = int(d['occludes'])
        init_CubePhysical(&self.cube_array[id], id, active, occludes)
        #self.cube_array[id] = CubePhysical(id, active, occludes)
        #init cube struct
        #self.cube_array[id].id = id
        #self.cube_array[id].active = active
        #self.cube_array[id].occludes = occludes

    cdef inline int isActive(CubePhysicalProperties self, unsigned int id):
        if id >= max_cubes: #max number of cubes
            return 0
        return self.cube_array[id].active

    cdef inline int isOcclude(CubePhysicalProperties self, unsigned int id):
        if id >= max_cubes: #max number of cubes
            return 0
        return self.cube_array[id].occludes

#the cache for cube visual properties
class CubeVisualProperties:
    def __init__(self):
        pass

    #def isActive(self, tile_id):
        #if self.cubes.has_key(tile_id):
            #return self.cubes[tile_id]['active']
        #else:
            #print "Error, cube type does not exist"
            #return False

    #def isOcclude(self, tile_id):
        #if self.cubes.has_key(tile_id):
            #return self.cubes[tile_id]['occludes']
        #else:
            #print "Error, cube type does not exist"
            #return False


#import ctypes

cdef enum:
    x_chunk_size = 8
    y_chunk_size = 8
    z_chunk_size = 8

class CubeGlobals:
    terrainMap = TerrainMap()
    mapChunkManager = MapChunkManager()

    cubePhysicalProperties = CubePhysicalProperties()
    cubeProperties = cubeProperties() #deprecated for visual properties
    textureGrid = None

    cubeRenderCache = None

    @classmethod
    def setTextureGrid(self, textureGrid):
        self.textureGrid = textureGrid
        self.cubeRenderCache = CubeRenderCache()

class MapChunkManager(object):
    draw_batch = pyglet.graphics.Batch()
    terrainMap = None

    def __init__(self):
        self.mapChunks = []
        self.mp = {}
        assert self.terrainMap != None
        MapChunk.terrainMap = self.terrainMap #assignment
        self.cubeProperties = CubeProperties()
        MapChunk.cubeProperties = CubeProperties()

    def set_map(self, int x, int y, int z):
        x = x - (x%x_chunk_size)
        y = y - (y%y_chunk_size)
        z = z - (z%z_chunk_size)
        t = (x,y,z)
        if self.mp.has_key(t):
            mp[t].update = True
        else:
            mp[t] = MapChunk(x,y,z)

    def regiser_chunk(self, mapChunk):
        self.mapChunks.append(mapChunk)

    def update_chunk(self):
        for mapChunk in self.mapChunks: #update only one buffer per frame
            if mapChunk.update == True:
                mapChunk.update_vertex_buffer(self.draw_batch)
                return

class MapChunk(object):

    terrainMap = CubeGlobals.terrainMap
    cubePhysicalProperties = CubeGlobals.cubePhysicalProperties
#    cubeProperties = None
    cubeRenderCache = None

    def __init__(self, x_offset, y_offset, z_offset):
        assert self.terrainMap != None
        self.vertexList = None #an in describing batch number
        self.x_offset = x_offset
        self.y_offset = y_offset
        self.z_offset = z_offset
        self.update = True
        self.empty = True
        self.CubeGlobals.mapChunkManager.register_chunk(self)

    def update_vertex_buffer(self, batch = None):
        cdef int tile_id, x, y, z
        cdef int active_cube_numer, culled_quads
        cdef int side_num


        draw_list = []
        active_cube_number = 0
        culled_quads = 0
        for x in range(self.x_offset, self.x_offset+x_chunk_size):
            for y in range(self.x_offset, self.x_offset +y_chunk_size):
                for z in range(self.x_offset, self.x_offset+z_chunk_size):
                    tile_id = self.terrainMap.get(x,y,z)
                    ###
                    if self.cubePhysicalProperties.isActive(tile_id): #non-active tiles are not draw
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

        cdef int v_num
        cdef float rx, ry, rz

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
            self.empty = True  #should do something!  Recycle chunk
            self.update = False
            return

        ## May cause issues
        if self.vertexList != None:
            self.vertexList.delete() #clean out old vertexList

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

    #cdef inline int _is_occluded(self,int x,int y,int z,int side_num):
    def _is_occluded(self,int x,int y,int z,int side_num):
        cdef int _x, _y, _z, tile_id

        s_array = [(0,0,1), (0,0,-1), (0,1,0), (0,-1,0), (-1,0,0),(1,0,0)] #replace with if/then statement
        temp = s_array[side_num]
        _x = temp[0] + x
        _y = temp[1] + y
        _z = temp[2] + z

        tile_id = self.terrainMap.get(_x,_y,_z)
        return self.cubePhysicalProperties.isOcclude(tile_id)


###

#deprecated by CubeVisualProperties
class CubeProperties(object):

    def __init__(self):
        pass

    def getTexture(self, tile_id, side_num):
        global cube_list
        if cube_list.has_key(tile_id):
            tex_a = cube_list[tile_id]['texture']
            return tex_a[side_num]
        else:
            return 0

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

class CubeRenderCache(object):

    def __init__(self):
        self.cubeProperties = CubeGlobals.cubeProperties
        self.textureGrid = CubeGlobals.textureGrid
        assert self.textureGrid != None
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
