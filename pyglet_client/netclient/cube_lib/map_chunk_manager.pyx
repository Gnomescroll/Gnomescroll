import settings

#if settings.pyglet:
import pyglet
from pyglet.gl import *

class MapChunkManagerGlobal:

    mapChunkManager = None
    #transparentBlockManager = None

    @classmethod
    def init_0(cls):
        cls.mapChunkManager = MapChunkManager()
        #cls.transparentBlockManager = TransparentBlockManager()
    @classmethod
    def init_1(cls):
        MapChunk.init()
        cls.mapChunkManager.init()
        #TransparentBlockManager.init()

from cube_dat import CubeGlobal

cimport terrain_map
from terrain_map cimport TerrainMap

cdef enum:
    x_chunk_size = 8
    y_chunk_size = 8
    z_chunk_size = 8

class TransparentBlockManager(object):
    cubePhysicalProperties = None
    terrainMap = None
    cubeRenderCache = None

    def __init__(self):
        self.reset()
    @classmethod
    def init(cls):
        cls.cubePhysicalProperties = CubeGlobal.cubePhysicalProperties
        cls.terrainMap = GameStateGlobal.terrainMap
        cls.cubeRenderCache = CubeGlobal.cubeRenderCache

    def reset(self):
        self.blocks = {}
        #self.v_list = []
        #self.c_list = []
        self.vertexList = None

    def update_block(self,x,y,z):
        pass

    def update_all_blocks(self):
        self.blocks = {}
        self.draw_list = []
        #cdef c MapChunk
        #cdef int i
        #cdef int tile_id
        cubePhysicalProperties =  CubeGlobal.cubePhysicalProperties
        for c, x_off, y_off, z_off in GameStateGlobal.terrainMap.get_chunk_list():
            for x in range(0,8):
                for y in range(0,8):
                    for z in range(0,8):
                        tile_id = c.get(x_off+x,y_off+y, z_off+z)
                        if cubePhysicalProperties.isTransparent(tile_id):
                            self.blocks[(x_off+x, y_off+y, z_off+z)] =  tile_id
        #update draw list
        for (x,y,z), tile_id in self.blocks.items():
            for side_num in [0,1,2,3,4,5]:
                if not _is_occluded2(self,x,y,z,side_num, tile_id):
                    self.draw_list.append((x,y,z,tile_id, side_num))

    def update_vbo(self):
        cdef int tile_id, side_num, x, y, z
        cdef int v_num
        cdef float rx, ry, rz

        v_list = []
        c_list = []
        tex_list = []
        v_num = 0
        for (x,y,z,tile_id, side_num) in self.draw_list:
            rx = x #should be floats
            ry = y
            rz = z

            (tv_list, tc_list, ttex_list) = self.cubeRenderCache.get_side(rx, ry, rz, tile_id, side_num)
            v_list += tv_list
            c_list += tc_list
            #tex_list += ttex_list
            v_num += 4

        if self.vertexList != None:
            self.vertexList.delete()
        if v_num == 0:
            self.vertexList = None
        else:

        #    pyglet.graphics.draw(v_num, GL_QUADS,
        #("v3f", v_list),
        #("c4B", c_list)
        #)

            self.vertexList = pyglet.graphics.vertex_list(v_num,
        ('v3f\static', v_list),
        ('c4B\static', c_list),
        #("t3f\static", tex_list),
        )

class MapChunkManager(object):
    terrainMap = None

    def init(self):
        self.terrainMap = GameStateGlobal.terrainMap
        assert self.terrainMap != None

    def __init__(self):
        if settings.pyglet:
            self.draw_batch = pyglet.graphics.Batch()
        self.mapChunks = []
        self.mp = {}

    def set_map(self, int x, int y, int z):
        x = x - (x%x_chunk_size)
        y = y - (y%y_chunk_size)
        z = z - (z%z_chunk_size)
        t = (x,y,z)
        if self.mp.has_key(t):
            self.mp[t].update = True
        else:
            self.mp[t] = MapChunk(x,y,z)

    def get_chunk_list(self):
        l = []
        for index in self.mp.keys():
            l.append(index)
        return l

    def register_chunk(self, mapChunk): #WTF calls this?
        self.mapChunks.append(mapChunk)

    def update_chunk(self):
        for mapChunk in self.mapChunks: #update only one buffer per frame
            if mapChunk.update == True:
                mapChunk.update_vertex_buffer(self.draw_batch)
                return

class MapChunk(object):

    terrainMap = None
    cubePhysicalProperties = None
    cubeRenderCache = None

    @classmethod
    def init(self):
        self.terrainMap = GameStateGlobal.terrainMap   #use world_state global
        self.cubePhysicalProperties = CubeGlobal.cubePhysicalProperties
        self.cubeRenderCache = CubeGlobal.cubeRenderCache
        assert self.terrainMap != None
        assert self.cubePhysicalProperties != None
        assert self.cubeRenderCache != None

    def __init__(self, x_off, y_off, z_off):
        self.vertexList = None #an in describing batch number

        self.x_off = x_off - (x_off % 8)
        self.y_off = y_off - (y_off % 8)
        self.z_off = z_off - (z_off % 8)

        self.update = True
        self.empty = True
        MapChunkManagerGlobal.mapChunkManager.register_chunk(self)

    def update_vertex_buffer(self, batch = None):
        cdef int tile_id, x, y, z
        cdef int active_cube_numer, culled_quads
        cdef int side_num


        draw_list = []
        active_cube_number = 0
        culled_quads = 0
        for x in range(self.x_off, self.x_off+x_chunk_size):
            for y in range(self.y_off, self.y_off +y_chunk_size):
                for z in range(self.z_off, self.z_off+z_chunk_size):
                    tile_id = self.terrainMap.get(x,y,z)
                    ###
                    if self.cubePhysicalProperties.isActive(tile_id) != 0: #non-active tiles are not draw
                        active_cube_number += 1
                        for side_num in [0,1,2,3,4,5]:
                            if not _is_occluded(self,x,y,z,side_num):
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
            rx = x #should be floats
            ry = y
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
cdef inline _is_occluded(self,int x,int y,int z,int side_num):
        cdef int _x, _y, _z, tile_id

        s_array = [(0,0,1), (0,0,-1), (0,1,0), (0,-1,0), (-1,0,0),(1,0,0)] #replace with if/then statement
        temp = s_array[side_num]
        _x = temp[0] + x
        _y = temp[1] + y
        _z = temp[2] + z

        tile_id = self.terrainMap.get(_x,_y,_z)
        return self.cubePhysicalProperties.isOcclude(tile_id)

cdef inline int _is_occluded2(self,int x,int y,int z,int side_num, int tile_id_in):
        cdef int _x, _y, _z, tile_id

        s_array = [(0,0,1), (0,0,-1), (0,1,0), (0,-1,0), (-1,0,0),(1,0,0)] #replace with if/then statement
        temp = s_array[side_num]
        _x = temp[0] + x
        _y = temp[1] + y
        _z = temp[2] + z

        tile_id = self.terrainMap.get(_x,_y,_z)
        if tile_id == tile_id_in:
            return 1
        return self.cubePhysicalProperties.isOcclude(tile_id)
