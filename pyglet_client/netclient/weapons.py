from game_objects import GameObject
from game_objects import EquippableObject

class Weapon(EquippableObject):

    _weapons = {
        'Weapon'    :   0,
        'LaserGun'  :   1,
        'Pick'      :   2,
        'BlockApplier':   3,
    }

    def __init__(self, id, owner=None):
        if id is None:
            raise ValueError
        self.id = id
        self.owner = owner
        self.type = self._weapons[self.__class__.__name__]

    def fire(self):
        return False

    def reload(self):
        return False

    def __str__(self):
        return self.__class__.__name__

    @classmethod
    def name_from_type(cls, type):
        for name, type_id in cls._weapons.items():
            if type_id == type:
                return name

    def hud_display(self):
        undef = '--'
        ammo = getattr(self, 'ammo', undef)
        max_ammo = getattr(self, 'max_ammo', undef)
        clip = getattr(self, 'clip', undef)
        clip_size = getattr(self, 'clip_size', undef)
        strfs = tuple([str(a) for a in [clip, clip_size, ammo, max_ammo]])
        return '%s/%s  ::  %s/%s' % strfs
        

class LaserGun(Weapon):

    def __init__(self, id=None, owner=None, clip=None, **kwargs):
        Weapon.__init__(self, id, owner)
        self.base_damage = 35
        self.clip_size = 20
        self.max_ammo = 100
        self.reload_speed = 750 #ms
        self.automatic = False
        self.firing_rate = 100 #ms

        if clip is None:
            clip = self.clip_size
        self.clip = clip
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
        self.ammo -= amt
        self.clip += amt
        return 'reload_weapon'


class BlockApplier(Weapon):

    def __init__(self, id=None, owner=None, clip=None, **kwargs):
        Weapon.__init__(self, id, owner)
        self.max_ammo = 100
        self.ammo = self.max_ammo
        self.clip_size = 100
        if clip is None:
            clip = self.clip_size
        self.clip = clip

    def fire(self):
        self.ammo -= 1
        return 'set_block'

    def restock(self, amt):
        self.ammo = min(self.max_ammo, self.ammo + amt)
        self.ammo = max(0, self.ammo)
        return 'restock_blocks'

class Pick(Weapon):

    def __init__(self, id, owner=None, **kwargs):
        Weapon.__init__(self, id, owner)

    def fire(self):
        return 'hit_block'

    def reload(self):
        return False
