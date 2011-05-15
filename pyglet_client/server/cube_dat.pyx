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

    cdef inline int isActive(CubePhysicalProperties self, unsigned int id):
        if id >= max_cubes: #max number of cubes
            return 0
        return self.cube_array[id].active

    cdef inline int isOcclude(CubePhysicalProperties self, unsigned int id):
        if id >= max_cubes: #max number of cubes
            return 0
        return self.cube_array[id].occludes

cdef enum:
    x_chunk_size = 8
    y_chunk_size = 8
    z_chunk_size = 8

class CubeGlobals:
    cubePhysicalProperties = CubePhysicalProperties()

    @classmethod
    def setTextureGrid(self, textureGrid):
        self.textureGrid = textureGrid
        self.cubeRenderCache = CubeRenderCache()
