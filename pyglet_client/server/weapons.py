from game_objects import GameObject
from game_objects import EquippableObject
from game_state import GenericMultiObjectList

class WeaponList(GenericMultiObjectList):

    def __init__(self):
        GenericMultiObjectList.__init__(self)
        self._allow_klasses(self, [ \
            LaserGun,
            Pick,
            BlockApplier,
        ])

    def create(self, klass_name, *args, **kwargs):
        return self._add(klass_name, *args, **kwargs)

    def destroy(self, obj):
        return self._remove(self, obj)
        
class Weapon(EquippableObject):

    _weapons = {
        'Weapon'    :   0,
        'LaserGun'  :   1,
        'Pick'      :   2,
        'BlockApplier':   3,
    }

    def __init__(self, id, owner=None):
        self.id = id
        self.owner = owner
        self.type = self._weapons[self.__class__.__name__]

    def fire(self):
        return False

    def reload(self):
        return False

    def __str__(self):
        return self.__class__.__name__

    def json(self):
        return {
            'type'  :   self.type,
        }

class LaserGun(Weapon):

    def __init__(self, id, owner=None):
        Weapon.__init__(self, id, owner)
        self.base_damage = 35
        self.clip_size = 20
        self.max_ammo = 100
        self.reload_speed = 750 #ms
        self.automatic = False
        self.firing_rate = 100 #ms

        self.clip = self.clip_size
        self.ammo = self.max_ammo

    def fire(self):
        if self.clip == 0:
            return False
        self.clip -= 1
        return 'fire_projectile'

    def reload(self):
        if self.ammo == 0:
            return False
        amt = min(self.clip_size - self.clip, self.ammo) # lesser of: filling the clip, or remaining ammo
        old_ammo = self.ammo
        old_clip = self.clip
        self.ammo -= amt
        self.clip += amt
        if old_ammo == self.ammo and old_clip == self.clip:
            return False
        return 'reload_weapon'

    def json(self):
        return {
            'type'  :   self.type,
            'clip'  :   self.clip,
            'ammo'  :   self.ammo,
        }

class BlockApplier(Weapon):

    def __init__(self, id, owner=None):
        Weapon.__init__(self, id, owner)
        self.max_ammo = 100
        self.clip_size = 100
        self.clip = self.clip_size

    def fire(self):
        return 'place_block'

    def json(self):
        return {
            'type'  :   self.type,
            'clip'  :   self.clip,
        }

class Pick(Weapon):

    def __init__(self, id, owner=None):
        Weapon.__init__(self, id, owner)

    def fire(self):
        return 'hit_block'

    def reload(self):
        return False
