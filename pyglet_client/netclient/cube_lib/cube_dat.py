#'''COPIED FROM SERVER'''
#def_tex_order = [[0,1,2,3],
                #[0,1,2,3],
                #[0,1,2,3],
                #[0,1,2,3],
                #[0,1,2,3],
                #[0,1,2,3],]

#cube_list = {
    #0 : {
        #'id' : 0,
        #'occludes' : False,
        #'active' : False,
        #'solid' : False,
        #},
#}

#def get_default_tile(i):

    #return {
        #'id'    :   i + 1,
        #'name'  :   'tile',
        #'occludes': True,
        #'active':   True,
        #'solid':    True,
        #'max_damage' : 32,
        #'neutron_tolerance' : 2,
        #'nuclear' : i%2,
        #'texture_id': [i]*6,
        #'texture_order': def_tex_order,
    #}

#def set_cube_list_defaults():
    #global cube_list

    #for i in range(256):
        #j = i+1
        #cube_list.setdefault(j, get_default_tile(i))

#set_cube_list_defaults()

from dat_loader import c_dat

cube_list = c_dat.dat
