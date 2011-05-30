from game_objects import GameObject
from game_objects import EquippableObject

        
class Weapon(EquippableObject):

    _weapons = {
        'Laser Gun' :   1,
        'Pick'      :   2,
        'Block'     :   3,
    }

    def __init__(self):
        pass

    def fire(self):
        return False

    def reload(self):
        return False

    def __str__(self):
        return 'Weapon Base Class'

    def key(self):
        return self._weapons[str(self)]

class LaserGun(Weapon):

    def __init__(self):
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

    def __str__(self):
        return 'Laser Gun'

    def key(self):
        return _weapons[str(self)]


class BlockApplier(Weapon):

    def __init__(self):
        self.max_ammo = 100
        self.clip_size = 100
        self.clip = self.clip_size

    def fire(self):
        return 'place_block'

    def __str__(self):
        return 'Block'

class Pick(Weapon):

    def __init__(self):
        pass

    def fire(self):
        return 'hit_block'

    def reload(self):
        return False

    def __str__(self):
        return 'Pick'
