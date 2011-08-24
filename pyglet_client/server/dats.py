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

items = {}

projectiles = {}
