from game_objects import GameObject
from game_objects import EquippableObject
from object_lists import GenericMultiObjectList

from utils import filter_props

import dat_loader
w_dat = dat_loader.w_dat

class WeaponList(GenericMultiObjectList):

    def __init__(self):
        GenericMultiObjectList.__init__(self)
        self._allow_klasses([ \
            LaserGun,
            Pick,
            BlockApplier,
            HitscanLaserGun,
            GrenadePouch,
            GrenadePouch_C,
        ])
        
class Weapon(EquippableObject):

    _weapons = {    # maps .__class__.__name__ to type identifier. must be the same on server and client.
        'Weapon'    :   0,
        'LaserGun'  :   1,
        'Pick'      :   2,
        'BlockApplier':   3,
        'HitscanLaserGun': 4,
        'GrenadePouch'  :   5,
        'GrenadePouch_C':   6,
    }

    dat = w_dat

    def __init__(self, id, owner=None):
        self.id = id
        self.owner = owner
        self._set_type()

        #self.max_ammo = self.get_dat('max_ammo')
        #self.ammo = self.get_dat('ammo')
        #self.clip_size = self.get_dat('clip_size')
        #self.clip = self.get_dat('clip')
        #self.base_damage = self.get_dat('base_damage')
        #self.automatic = self.get_dat('automatic')
        #self.hitscan = self.get_dat('hitscan')
        #self.reload_speed = self.get_dat('reload_speed')
        #self.firing_rate = self.get_dat('firing_rate')

        self.dat.apply(self)

        self.fire_command = ''

    def _set_type(self):
        self.type = self._weapons[self.__class__.__name__]
        
    def fire(self):
        if self.clip == 0:
            return False
        self.clip -= 1
        return self.fire_command

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
        self.fire_command = 'fire_projectile'

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
        self.fire_command = 'set_block'

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
        self.damage = 35

    def fire(self):
        return self.fire_command

    def reload(self):
        return False


class GrenadePouch(Weapon):

    def __init__(self, id, owner=None, **kwargs):
        Weapon.__init__(self, id, owner, **kwargs)
        self.fire_command = 'throw_grenade'

    def hud_display(self):
        fmt = self.hud_display_format_string()
        strfs = (self.clip, self.clip_size, self._hud_undef, self._hud_undef,)
        return fmt % strfs

    def reload(self):
        return False

    def json(self, properties=None):
        d = Weapon.json(self, properties)
        if properties is None:
            d.update({
                'clip'  :   self.clip,
            })
        else:
            d.update(filter_props(self, properties))
        return d


class GrenadePouch_C(GrenadePouch):

    def __init__(self, id, owner=None, **kwargs):
        GrenadePouch.__init__(self, id, owner)
        self.fire_command = 'throw_grenade_c'

from net_out import NetOut
