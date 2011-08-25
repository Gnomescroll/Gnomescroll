'''
Dat files
'''

weapons = {

    0   :   {   # Weapon    (serves as defaults)
        'max_ammo'  :   0,
        'ammo'      :   0,
        'clip_size' :   0,
        'clip'      :   0,
        'base_damage':  0,
        'automatic' :   False,
        'hitscan'   :   False,
        'reload_speed'  :   500, #ms
        'firing_rate'   :   500, #ms
    },

    1   :   {   # LaserGun
        'max_ammo'  :   100,
        'ammo'      :   100,
        'clip_size' :   100,
        'clip'      :   100,
        'base_damage':   35,
        'reload_speed': 750, #ms
        'firing_rate':  330, #ms
    },

    2   :   {   # Pick
        'base_damage'   :   0,
    },

    3   :   {   # BlockApplier
        'max_ammo'  :   0,
        'ammo'      :   0,
        'clip_size' :   9999,
        'clip'      :   9999,
    },
    
    4   :   {   # HitscanLaserGun
        'max_ammo'  :   100,
        'ammo'      :   100,
        'clip_size' :   100,
        'clip'      :   100,
        'base_damage':   35,
        'reload_speed': 750, #ms
        'firing_rate':  330, #ms
        'hitscan'   :   True,
    },

    5   :   {   # GrenadePouch
        'max_ammo'  :   100,
        'ammo'      :   0,
        'clip_size' :   100,
        'clip'      :   100,
    },
    
    6   :   {   # GrenadePouch_C
        'max_ammo'  :   100,
        'ammo'      :   0,
        'clip_size' :   100,
        'clip'      :   100,
    },
}


'''
'''

items = {

    0   :   {   # GameObject
        'radius'    :   1,
        'immobile'  :   False,
        'auto_grab' :   False,
        'drop_on_death' :   False,
        'pickup_by_own_team': False,
        'pickup_by_other_team': False,
        'proximity_effect'  :   False,
    },

        
    1   :   {   # StaticObject
        'immobile'  :   True,
    },

    2   :   {   # EquippableObject
    },

    3   :   {   #   DetachableObject
        'drop_on_death' :   True,
    },
    
    4   :   {   # Flag
        'auto_grab' :   True,
        'drop_on_death':    True,
        'pickup_by_other_team'  :   True,
    },

    5   :   {   # Base
        'immobile'  :   True,
        'radius'    :   2,
        'proximity_effect': True,
    },

}

'''
'''

projectiles = {

    0   :   {   # generic projectile
        'speed' :   0,
        'damage':   0,
        'ttl_max':  0,
        'penetrates': False,
        'suicidal'  : False,
        'splash'    : False,
        'splash_radius':    0,
        'splash_damage':    0,
        'splash_force':     0,
    },

    1 : {   # laser
        'speed' : 100,
        'damage' : 20,
        'ttl_max' : 400, #time to live in ticks
    },

    2 : {   #   grenade
        'speed' : 50,
        'damage' : 50,
        'ttl_max' : 300,
        'suicidal': True,
        'splash'    :   True,
        'splash_radius':    3,
        'splash_damage':    15,
        'splash_force':     150,
    },

}

'''
'''

cubes = {
    0 : {
        #'id' : 0,
        'name': 'tile',
        'occludes' : False,
        'active' : False,
        'solid' : False,
        'max_damage': 32,
        'neutron_tolerance' : 2,
        'nuclear': 0,
        'texture_id': [0,0,0,0,0,0],
        'texture_order': [\
            [0,1,2,3],
            [0,1,2,3],
            [0,1,2,3],
            [0,1,2,3],
            [0,1,2,3],
            [0,1,2,3],
        ],
        'gravity'   :   0,
        'transparent': 0,
    },
}

def _build_cubes():
    def_tex_order = [[0,1,2,3],
                [0,1,2,3],
                [0,1,2,3],
                [0,1,2,3],
                [0,1,2,3],
                [0,1,2,3],]
                
    def get_default_tile(i):

        return {
            #'id'    :   i + 1,
            'name'  :   'tile',
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
        global cubes

        for i in range(256):
            j = i+1
            cubes.setdefault(j, get_default_tile(i))

    set_cube_list_defaults()
_build_cubes()
