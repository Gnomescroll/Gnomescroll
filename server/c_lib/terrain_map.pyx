
cdef extern from "../c_lib/t_map/t_map.hpp" namespace "t_map":
    void init_t_map()
    int _get(int x, int y, int z)
    void _set(int x, int y, int z, int value)

#called automaticly after properties are loaded
def _init_map():
    init_t_map()



#int _clear()
#int _apply_damage(int x, int y, int z, int value)
#int _get_highest_open_block(int x, int y, int n)
#int _get_lowest_open_block(int x, int y, int n)
#void set_map_size(int x, int y, int z)

'''
PART 2: Properties
'''

cdef extern from "./t_map/t_properties.hpp" namespace "t_map":
    struct cubeProperties:
        bint active
        bint solid
        bint occludes
        bint transparent
        bint reserved5
        bint reserved6
        bint reserved7
        bint reserved8
        unsigned char max_damage

cdef extern from "./t_map/t_properties.hpp" namespace "t_map":
    cubeProperties* get_cube(int id)

'''
Part 1: State

'''

cpdef inline set(int x,int y, int z,int value):
    _set(x,y,z,value)

cpdef inline int get(int x, int y,int z):
    return _get(x,y,z)


from dat_loader import c_dat

def init_cube_properties(id=None):
    global c_dat

    def apply(id):
        global infinite_texture_counter
        cdef cubeProperties* cp
        cp = get_cube(id)
        cp.active = int(c_dat.get(id,'active'))
        cp.solid = int(c_dat.get(id,'solid'))
        cp.occludes = int(c_dat.get(id,'occludes'))
        cp.transparent = int(c_dat.get(id,'transparent'))
        cp.max_damage = int(c_dat.get(id,'max_damage'))

    if id is None:
        for id in c_dat.dat:
            apply(id)
    else:
        apply(id)

'''
    Init Stuff
'''

def init():
    global c_dat
    print "Init Terrain Map"
    init_cube_properties(c_dat)
    init_t_map()
init()



#deprecated below line



'''  
cdef extern from "./t_map/t_compress.h":
    int map_save_to_disk(char* fn)
cdef extern from "./t_map/t_decompress.h":
    int map_load_from_disk(char* fn)
'''

'''
def clear():
    _clear()

# n defines how many continuous open spaces must exist
def get_highest_open_block(int x, int y, int n=1):
    return _get_highest_open_block(x,y,n)

def get_lowest_open_block(int x, int y, int n=1):
    return _get_lowest_open_block(x,y,n)

def set_map_dimensions(int x, int y, int z):
    set_map_size(x,y,z);

def send_map_metadata_to_client(int id):
    send_map_metadata(id)
'''

'''
Part 5: Serialization
'''


'''
import os, os.path, time
map_file_location = './content/maps/%s'

def save_to_disk(fn=''):

    if not fn:
        fn = 'map_%0.2f' % (time.time())
        fn = fn.replace('.', '_')
    fn = fn.replace('/', '_')
    fn = map_file_location % (fn,)

    map_save_to_disk(fn);

def load_from_disk(fn=''):
    if not fn:
        fn = 'mapsave1'
        
    fn = map_file_location % (fn,)
    if not os.path.exists(fn):
        print "Map %s does not exist. Abort map load." % (fn,)
        return 1
        
    map_load_from_disk(fn)
'''