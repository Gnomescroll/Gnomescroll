
#from world_state import WorldStateGlobal

from cube_lib.types cimport *

import settings

class CubeGlobal:
    #terrainMap = TerrainMap()
    cubePhysicalProperties = None
    #collisionDetection = None
    #client specific
    cubeRenderCache = None
    textureGrid = None

    @classmethod
    def init_0(cls):
        #cls.cubePhysicalProperties = CubePhysicalProperties()
        global cubePhysicalProperties
        cubePhysicalProperties.CubePhysicalProperties
        cubePhysicalProperties.init()
        cls.cubePhysicalProperties = cubePhysicalProperties
        #global collisionDetection
        #cls.collisionDetection = collisionDetection
        #cls.cubeRenderCache = CubeRenderCache()
    @classmethod
    def init_1(cls):
        pass
        #global collisionDetection
        #collisionDetection.init()
        #cls.collisionDetection.init()
#    @classmethod
#    def setTextureGrid(cls, texture_grid):
        #print "set texture grid"
        #cls.textureGrid = texture_grid
        #cls.cubeRenderCache.set_texture(texture_grid)

#from map_chunk_manager import MapChunkManagerGlobal

cimport terrain_map
#from terrain_map cimport TerrainMap

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
        'active' : False, #should be drawn
        'solid' : False,
        'transparent' : True,
        'gravity' : 5, #for anti-grav

        'texture' : [ #t, b, w, e, n, s
        (1, [0,1,2,3]),  #top
        (1, [0,1,2,3]),  #bottom
        (1, [0,1,2,3]), #west
        (1, [0,1,2,3]), #east
        (1, [0,1,2,3]), #north
        (1, [0,1,2,3]), #south
        ],
        'color' : [0,0,155,150], #alpha
    },
 }


cdef struct CubePhysical:
    int id
    int active
    int occludes
    int solid
    int gravity
    int transparent

#used for initing the struct
cdef void init_CubePhysical(CubePhysical*x, int id, int active, int occludes, int solid, int gravity, int transparent):
    x.id = id
    x.active = active
    x.occludes = occludes
    x.solid = solid
    x.gravity = gravity
    x.transparent = transparent

#cdef enum:
#    max_cubes = 1024

def init():
    global cube_list
    for cube in cube_list.values():
        add_cube(cube)

cdef CubePhysical cube_array[max_cubes] #cube state

def add_cube(self, d):
    id = int(d['id'])
    if id >= max_cubes: #max number of cubes
        print "Error: cube id is too high"
        return
    active = int(d.get('active',1))
    occludes = int(d.get('occludes', 0))
    solid = int(d.get('solid', 1))
    gravity = int(d.get('gravity', 0))
    transparent = int(d.get('transparent', 0))
    init_CubePhysical(&cube_array[id], id, active, occludes, solid, gravity, transparent)

#!!!should not need to be cp
cpdef inline int isActive(CubePhysicalProperties self, unsigned int id):
    if id >= max_cubes: #max number of cubes
        return 0
    return self.cube_array[id].active
#!!!should not need to be cp
cpdef inline int isOcclude(CubePhysicalProperties self, unsigned int id):
    if id >= max_cubes: #max number of cubes
        return 0
    return self.cube_array[id].occludes

cpdef inline int isTransparent(CubePhysicalProperties self, unsigned int id):
    if id >= max_cubes: #max number of cubes
        return 0
    return self.cube_array[id].transparent

cpdef inline int isSolid(CubePhysicalProperties self, unsigned int id):
    if id >= max_cubes: #max number of cubes
        return 0
    return self.cube_array[id].solid


cdef CubePhysicalProperties* cubePhysicalProperties
#cdef CollisionDetection collisionDetection

### Cube Utilities ###

cpdef inline int collisionDetection(int x, int y, int z):
    cdef int tile
    tile = terrain_map.get(x,y,z)
    return cubePhysicalProperties.isSolid(tile)

#cdef class CollisionDetection:
#    pass

##the cache for cube visual properties
##deprecates CubeRenderCache
#class CubeVisualProperties:
    #def __init__(self):
        #pass

#cdef enum:
    #x_chunk_size = 8
    #y_chunk_size = 8
    #z_chunk_size = 8

####

#def convert_index(index, height, width):
    #index = int(index)
    #height = int(height)
    #width = int(width)
    #x_ = index % width
    #y_ = int((index - x_) / width)
    #y = height - y_ -1
    #rvalue =  x_ + y*width
    ##print "rvalue= " + str(rvalue)
    #return rvalue

#class CubeRenderCache(object):

    #def set_texture(self, textureGrid):
        #self.textureGrid = textureGrid
        #self.reset_cache()

    #def reset_cache(self):
        #self.c4b_cache = {}
        #self.t4f_cache = {}

    #def __init__(self):
        #self.textureGrid = None
        #self.reset_cache()

        #self.v_index = [
        #[ 0,1,1 , 0,0,1 , 1,0,1 , 1,1,1 ], #top
        #[ 1,0,0 , 0,0,0 , 0,1,0 , 1,1,0 ], #bottom
        #[ 0,1,1 , 1,1,1 , 1,1,0 , 0,1,0 ], #north
        #[ 0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 ], #south
        #[ 0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 ], #west
        #[ 1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 ], #east
    #]

        #north_side = [ [0,1,1],[1,1,1],[1,1,0],[0,1,0] ]
        #south_side = [[0,0,1],[0,0,0],[1,0,0],[1,0,1]]
        #west_side = [[0,1,1],[0,1,0],[0,0,0],[0,0,1]]
        #east_side = [[1,0,1],[1,0,0],[1,1,0],[1,1,1]]
        #top_side = [[0,1,1],[0,0,1],[1,0,1],[1,1,1]]
        #bottom_side = [[1,0,0],[0,0,0],[0,1,0],[1,1,0]]

    ### t, b, n, s, w, e
    #def get_side(self, x, y, z, tile_id, side_num):
        #ta = self.v_index[side_num]
        ##v_list = (GLfloat * 12) [ta[0]+x,ta[1]+y,ta[2]+z , ta[3]+x,ta[4]+y,ta[5]+z , ta[6]+x,ta[7]+y,ta[8]+z , ya[9]+x,ta[10]+y,ta[11]+z ]
        #v_list = [ta[0]+x,ta[1]+y,ta[2]+z , ta[3]+x,ta[4]+y,ta[5]+z , ta[6]+x,ta[7]+y,ta[8]+z , ta[9]+x,ta[10]+y,ta[11]+z ]
        #c4B_list = self._get_c4B(tile_id, side_num)
        #t4f_list = self._get_t4f(tile_id, side_num)
        #return(v_list, c4B_list, t4f_list)

    #def _get_c4B(self, tile_id, side_num):
        #if self.c4b_cache.has_key((tile_id, side_num)):
            #return self.c4b_cache[(tile_id, side_num)]
        #else:
            ###compute from dict!
            ##temp = (GLbyte * 4)[255, 255, 255, 255] * 4
            #rgba = cube_list[tile_id].get('color', [255, 255, 255, 255])
            #temp = rgba * 4

            #self.c4b_cache[(tile_id, side_num)] = temp
            #return temp

    #def _get_t4f(self, tile_id, side_num):
        #if self.t4f_cache.has_key((tile_id, side_num)):
            #return self.t4f_cache[(tile_id, side_num)]
        #else:
            #global cube_list  #need to put this somewhere
            #if cube_list.has_key(tile_id):
                #tex_a = cube_list[tile_id]['texture']
                #(texture_id, rotation) = tex_a[side_num]
            #else:
                #print "CubeRenderCache error, cube does not exist"
            #tex_tuple = self.textureGrid[convert_index(texture_id, 16, 16)].tex_coords

            #self.t4f_cache[(tile_id, side_num)] = list(tex_tuple)
            #return list(tex_tuple)
