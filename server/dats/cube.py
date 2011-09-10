'''
dat = {
    0 : {
        'name': 'tile0',
        'occludes' : False,
        'active' : False,
        'solid' : False,
        'max_damage': 32,
        'neutron_tolerance' : 2,
        'nuclear': 0,
        'texture_id': [-1]*6,
        'texture_order': [[0,1,2,3]] * 6,
        'gravity'   :   0,
        'transparent': 0,
    },

    2 : {
            'name'  :   'tile2',
            'occludes': True,
            'active':   True,
            'solid':    True,
            'max_damage' : 32,
            'neutron_tolerance' : 2,
            'nuclear' : 0,
            'texture_id': [2]*6,
            'gravity'   :   0,
            'transparent': 0,
    },
}
'''
#from cube_dat import dat

import cube_dat

dat = cube_dat.dat

def _build_cubes():
    def_tex_order = [[0,1,2,3],
                [0,1,2,3],
                [0,1,2,3],
                [0,1,2,3],
                [0,1,2,3],
                [0,1,2,3],]

    def get_default_tile(i):

        return {
            'name'  :   'tile%d' % (i+1,),
            'occludes': True,
            'active':   True,
            'solid':    True,
            'max_damage' : 32,
            'neutron_tolerance' : 2,
            'nuclear' : i%2,
            'texture_id': [i]*6,
            'texture_order': def_tex_order,
            'gravity'   :   0,
            'transparent': 0,
        }

    def set_cube_list_defaults():
        global dat

        for i in range(256):
            j = i+1
            dat.setdefault(j, get_default_tile(i))

    set_cube_list_defaults()
_build_cubes()

# module reload behavior
try:
    _init
except NameError:
    _init = None
else:
    reload(cube_dat)
    dat.update(cube_dat.dat)

