from game_objects import GameObject
from game_objects import EquippableObject
from object_lists import GenericMultiObjectList

from utils import filter_props

class WeaponList(GenericMultiObjectList):

    def __init__(self):
        GenericMultiObjectList.__init__(self)
        self._allow_klasses([ \
            LaserGun,
            Pick,
            BlockApplier,
            HitscanLaserGun,
        ])
        
class Weapon(EquippableObject):

    _weapons = {
        'Weapon'    :   0,
        'LaserGun'  :   1,
        'Pick'      :   2,
        'BlockApplier':   3,
        'HitscanLaserGun': 4,
    }

    def __init__(self, id, owner=None):
        self.id = id
        self.owner = owner
        self.type = self._weapons[self.__class__.__name__]
        self.hitscan = False

        self.base_damage = 0
        self.clip_size = 0
        self.max_ammo = 0
        self.reload_speed = 1000 #ms
        self.automatic = False
        self.firing_rate = 1000 #ms
        self.clip = self.clip_size
        self.ammo = self.max_ammo

        self.fire_command = ''
        
    def fire(self):
        return False

    def reload(self):
        return False

    def refill(self):
        old = (self.ammo, self.clip,)
        self.ammo = self.max_ammo
        self.clip = self.clip_size
        if old != (self.ammo, self.clip,):
            # ignore irrelevant properties. assume that if these are 0, they dont matter for the weapon
            props = []
            if self.ammo:
                props.append('ammo')
            if self.clip:
                props.append('clip')
            NetOut.event.weapon_update(self, props)

    def __str__(self):
        return self.__class__.__name__

    def json(self, properties=None):
        d = {
            'id'    :   self.id,
            'type'  :   self.type,
        }
        if properties is None:
            d.update({
                'owner'  :   self.owner.id,
            })
        else:
            d.update(filter_props(self, properties))
        return d

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

        self.fire_command = 'fire_projectile'

    def fire(self):
        if self.clip == 0:
            return False
        self.clip -= 1
        return self.fire_command

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

    def json(self, properties=None):
        d = Weapon.json(self, properties)
        if properties is None:
            d.update({
                #'type'  :   self.type,
                'clip'  :   self.clip,
                'ammo'  :   self.ammo,
            })
        else:
            d.update(filter_props(self, properties))
        return d

class HitscanLaserGun(LaserGun):

    def __init__(self, id=None, owner=None, **kwargs):
        LaserGun.__init__(self, id=id, owner=owner, **kwargs)
        self.hitscan = True
        self.fire_command = 'hitscan'

    def fire(self):
        return LaserGun.fire(self)

class BlockApplier(Weapon):

    def __init__(self, id, owner=None):
        Weapon.__init__(self, id, owner)
        self.max_ammo = 0
        self.ammo = 0
        self.clip_size = 100
        self.clip = self.clip_size
        self.fire_command = 'set_block'

    def fire(self):
        if self.clip == 0:
            return False
        self.clip -= 1
        return self.fire_command

    def json(self, properties=None):
        d = Weapon.json(self, properties)
        if properties is None:
            d.update({
                #'type'  :   self.type,
                'clip'  :   self.clip,
            })
        else:
            d.update(filter_props(self, properties))
        return d

class Pick(Weapon):

    def __init__(self, id, owner=None):
        Weapon.__init__(self, id, owner)
        self.fire_command = 'hit_block'

    def fire(self):
        return self.fire_command

    def reload(self):
        return False

from net_out import NetOut
