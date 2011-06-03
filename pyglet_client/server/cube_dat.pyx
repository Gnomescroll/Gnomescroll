import pyglet
from pyglet.gl import *

class CubeGlobal:
    cubePhysicalProperties = None
    CollisionDetection = None

    @classmethod
    def init_0(cls):
        cls.cubePhysicalProperties = CubePhysicalProperties()
        cls.collisionDetection = CollisionDetection()
    @classmethod
    def init_1(cls):
        cls.collisionDetection.init()
    @classmethod
    def setTextureGrid(cls, textureGrid):
        cls.textureGrid = textureGrid
        cls.cubeRenderCache = CubeRenderCache()

cimport terrain_map
from terrain_map cimport TerrainMap

#from terrain_map import TerrainMap

from game_state import GameStateGlobal

cube_list = {
    0 : {
        'id' : 0,
        'occludes' : False,
        'active' : False,
        'solid' : False,
        },
    1 : {
        'id' : 1,
        'occludes' : True,
        'active' : True,
        'solid' : True,

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
        'solid' : True,

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
        'solid' : True,

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
        'solid' : True,

        'texture' : [ #t, b, w, e, n, s
        (16, [0,1,2,3]),  #top
        (16, [0,1,2,3]),  #bottom
        (16, [0,1,2,3]), #west
        (16, [0,1,2,3]), #east
        (16, [0,1,2,3]), #north
        (16, [0,1,2,3]), #south
        ],
    },
    5 : {
        'id' : 5,
        'occludes' : False, #translucent
        'active' : True, #should be drawn
        'solid' : False,

        'gravity' : 5, #for anti-grav

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
    int solid
    int gravity

#used for initing the struct
cdef void init_CubePhysical(CubePhysical*x, int id, int active, int occludes, int solid, int gravity):
    x.id = id
    x.active = active
    x.occludes = occludes
    x.solid = solid
    x.gravity = gravity

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
        active = int(d.get('active',1))
        occludes = int(d.get('occludes', 0))
        solid = int(d.get('solid', 1))
        gravity = int(d.get('gravity', 0))
        init_CubePhysical(&self.cube_array[id], id, active, occludes, solid, gravity)

    cdef inline int isActive(CubePhysicalProperties self, unsigned int id):
        if id >= max_cubes: #max number of cubes
            return 0
        return self.cube_array[id].active

    cdef inline int isOcclude(CubePhysicalProperties self, unsigned int id):
        if id >= max_cubes: #max number of cubes
            return 0
        return self.cube_array[id].occludes

    cdef inline int isSolid(CubePhysicalProperties self, unsigned int id):
        if id >= max_cubes: #max number of cubes
            return 0
        return self.cube_array[id].solid

cdef class CollisionDetection:
    cdef TerrainMap terrainMap
    cdef CubePhysicalProperties cubePhysicalProperties

    def init(self):
        self.terrainMap = GameStateGlobal.terrainMap
        self.cubePhysicalProperties = CubeGlobal.cubePhysicalProperties

    def __init__(self):
        pass

    cpdef inline int collision(CollisionDetection self, int x, int y, int z):
        cdef int tile
        tile = self.terrainMap.get(x,y,z)
        return self.cubePhysicalProperties.isSolid(tile)

cdef enum:
    x_chunk_size = 8
    y_chunk_size = 8
    z_chunk_size = 8
