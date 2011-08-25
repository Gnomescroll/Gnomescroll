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
        'id' : 0,
        'occludes' : False,
        'active' : False,
        'texture': [ #t, b, w, e, n, s
            (0, []),  #top
            (0, []),  #bottom
            (0, []), #west
            (0, []), #east
            (0, []), #north
            (0, []), #south
        ],
    },
    1 : {
        'id' : 1,
        'occludes' : True,
        'active' : True,
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
