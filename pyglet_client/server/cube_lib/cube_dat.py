def_tex_order = [[0,1,2,3],
                [0,1,2,3],
                [0,1,2,3],
                [0,1,2,3],
                [0,1,2,3],
                [0,1,2,3],]

cube_list = {
    0 : {
        'id' : 0,
        'occludes' : False,
        'active' : False,
        'solid' : False,
        'max_damage' : 1,
        },
    #1 : {
        #'id' : 1,
        #'occludes' : True,
        #'active' : True,
        #'solid' : True,

        #'texture_id' : [1,1,1,1,1,1],
        #'texture_order' : [[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],]
        #},
    #2 : {
        #'id' : 2,
        #'occludes' : True,
        #'active' : True,
        #'solid' : True,

        #'texture_id' : [0,6,7,3,4,5],
        #'texture_order' : [[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],],
    #},
    #3 : {
        #'id' : 3,
        #'occludes' : True,
        #'active' : True,
        #'solid' : True,

        #'texture_id' : [1,1,1,1,1,1],
        #'texture_order' : [[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],],
    #},
    #4 : {
        #'id' : 4,
        #'occludes' : True,
        #'active' : True,
        #'solid' : True,

        #'texture_id' : [16,16,16,16,16,16],
        #'texture_order' : [[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],],
    #},
    #5 : {
        #'id' : 5,
        #'occludes' : False, #translucent
        #'active' : False, #should be drawn
        #'solid' : False,
        #'transparent' : True,
        #'gravity' : 5, #for anti-grav

        #'texture_id' : [1,1,1,1,1,1],
        #'texture_order' : [[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],],

        #'color' : [0,0,155,150], #alpha
    #},
    #6 : {
        #'id' : 6,
        #'name' : 'black rune',
        #'occludes' : True,
        #'active' : True,
        #'solid' : True,

        #'texture_id' : [144,144,144,144,144,144],
        #'texture_order' : [[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],
                            #[0,1,2,3],],
    #},
 }

def get_default_tile(i):

    return {
        'id'    :   i + 1,
        'name'  :   'tile',
        'occludes': True,
        'active':   True,
        'solid':    True,
        'texture_id': [i]*6,
        'texture_order': def_tex_order,
        'max_damage' : 3,
    }

def set_cube_list_defaults():
    global cube_list

    for i in range(256):
        j = i+1
        cube_list.setdefault(j, get_default_tile(i))

set_cube_list_defaults()



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
