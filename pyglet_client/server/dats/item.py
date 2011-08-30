dat = {

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
