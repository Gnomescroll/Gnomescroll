dat = {

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
        'speed' : 15,
        'damage' : 50,
        'ttl_max' : 100,
        'suicidal': True,
        'splash'    :   True,
        'splash_radius':    3,
        'splash_damage':    15,
        'splash_force':     150,
    },

}
