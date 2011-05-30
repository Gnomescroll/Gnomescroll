from game_objects import GameObject
from game_objects import EquippableObject

class Weapon(EquippableObject):

    def __init__(self):
        pass

    def fire(self):
        return False

    def reload(self):
        return False

    def __str__(self):
        return 'Weapon Base Class'

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
        if self.current_clip == 0:
            return False
        self.current_clip -= 1
        return 'fire_projectile'

    def reload(self):
        if self.ammo == 0:
            return False
        amt = min(self.clip_size - self.clip, self.ammo) # lesser of: filling the clip, or remaining ammo
        self.ammo -= amt
        self.clip += amt
        return 'reload_laser_gun'

    def __str__(self):
        return 'Laser Gun'


class Pick(Weapon):

    def __init__(self):
        pass

    def fire(self):
        return 'hit_block'

    def reload(self):
        return False

    def __str__(self):
        return 'Pick'
