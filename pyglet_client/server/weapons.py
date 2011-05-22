from game_objects import GameObject
from game_objects import EquippableObject

class Weapon(EquippableObject):

    def __init__(self):
        pass

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
        return True

    def reload(self):
        if self.ammo == 0:
            return False
        amt = min(self.clip_size - self.clip, self.ammo) # lesser of: filling the clip, or remaining ammo
        self.ammo -= amt
        self.clip += amt
        return True
        
