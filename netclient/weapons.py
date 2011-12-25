import animations

from game_objects import GameObject, EquippableObject
from dat_loader import w_dat

class Weapon(EquippableObject):

    _types = {
        'Weapon'    :   0,
        'Pick'      :   2,
        'BlockApplier':   3,
        'HitscanLaserGun': 4,
        'GrenadePouch'  :   5,
    }

    _names = None

    dat = w_dat

    _hud_undef = '--'

    def __init__(self, id, owner=None, state=None):
        EquippableObject.__init__(self, id, state)
        self.owner = owner
        self._set_type()
        self.dat.apply(self)
        self._animation = animations.Animation

    def fire(self):
        return False

    def reload(self):
        return False

    def __str__(self):
        return self.__class__.__name__

    def _set_type(self):
        self.type = self._types[self.__class__.__name__]

    @classmethod
    def name_from_type(cls, type):
        if cls._names is None:
            rev = [(b,a) for a,b in cls._types.items()]
            cls._names = dict(rev)
        return cls._names[type]

    def hud_display(self):
        undef = self._hud_undef
        ammo = getattr(self, 'ammo', undef)
        max_ammo = getattr(self, 'max_ammo', undef)
        clip = getattr(self, 'clip', undef)
        clip_size = getattr(self, 'clip_size', undef)
        strfs = (clip, clip_size, ammo, max_ammo,)
        return self.hud_display_format_string() % strfs

    def hud_display_format_string(self):
        return '%s/%s::%s/%s'

    def _update_info(self, **weapon):
        args = []
        if 'id' in weapon:
            args.append(self.id)
            self.id = weapon['id']
        if 'owner' in weapon:
            self.owner = weapon['owner']
        if 'type' in weapon:
            self.type = weapon['type']

        return args

    def update_info(self, **weapon):
        args = self._update_info(**weapon)
        GameStateGlobal.weaponList.update(self, *args)

    def animation(self, target=None, agent=None):
        return self._animation()


class HitscanLaserGun(Weapon):

    def __init__(self, id=None, owner=None, clip=None, state=None, **kwargs):
        Weapon.__init__(self, id, owner, state)
        if clip is None:
            clip = self.clip_size
        self.clip = clip
        self._animation = animations.HitscanLaserGunAnimation

    def fire(self):
        if self.clip == 0:
            return False
        self.clip -= 1
        return 'hitscan'

    def reload(self):
        if self.ammo == 0:
            return False
        amt = min(self.clip_size - self.clip, self.ammo) # lesser of: filling the clip, or remaining ammo
        self.ammo -= amt
        self.clip += amt
        return 'reload_weapon'

    #def animation(self, target=None, agent=None, vector=None):
        #if agent is None:
            #agent = GameStateGlobal.agentList[self.owner]
        #origin = agent.camera_position()
        #if vector is None:
            #vector = agent.direction()
        #anim = self._animation(origin, vector, target)
        #return anim

    def update_info(self, **weapon):
        args = self._update_info(**weapon)
        if 'clip' in weapon:
            self.clip = weapon['clip']
        if 'ammo' in weapon:
            self.ammo = weapon['ammo']
        if 'clip_size' in weapon:
            self.clip_size = weapon['clip_size']
        if 'max_ammo' in weapon:
            self.max_ammo = weapon['max_ammo']
        if 'base_damage' in weapon:
            self.base_damage = weapon['base_damage']
        if 'reload_speed' in weapon:
            self.reload_speed = weapon['reload_speed']
        if 'automatic' in weapon:
            self.automatic = bool(weapon['automatic'])
        if 'firing_rate' in weapon:
            self.firing_rate = weapon['firing_rate']
        if 'scope' in weapon:
            self.scope = bool(weapon['scope'])
        GameStateGlobal.weaponList.update(*args)
    

class BlockApplier(Weapon):

    def __init__(self, id=None, owner=None, clip=None, state=None, **kwargs):
        Weapon.__init__(self, id, owner, state)
        if clip is None:
            clip = self.clip_size
        self.clip = clip

    def fire(self):
        if self.clip == 0:
            return False
        #self.clip -= 1
        return 'set_block'

    def hud_display(self):
        fmt = self.hud_display_format_string()
        strfs = (self.clip, self.clip_size, self._hud_undef, self._hud_undef,)
        return fmt % strfs

    def update_info(self, **weapon):
        args = self._update_info(**weapon)
        if 'clip' in weapon:
            self.clip = weapon['clip']
        #if 'ammo' in weapon:
            #self.ammo = weapon['ammo']
        if 'clip_size' in weapon:
            self.clip_size = weapon['clip_size']
        #if 'max_ammo' in weapon:
            #self.max_ammo = weapon['max_ammo']
        GameStateGlobal.weaponList.update(*args)

class Pick(Weapon):

    def __init__(self, id, owner=None, state=None, **kwargs):
        Weapon.__init__(self, id, owner, state)

    def fire(self):
        return 'hit_block'

    def reload(self):
        return False


class GrenadePouch(Weapon):

    def __init__(self, id, owner=None, state=None, **kwargs):
        Weapon.__init__(self, id, owner, state)

    def fire(self):
        if self.clip == 0:
            return
        self.clip -= 1
        return 'throw_grenade'

    def hud_display(self):
        fmt = self.hud_display_format_string()
        strfs = (self.clip, self.clip_size, self._hud_undef, self._hud_undef,)
        return fmt % strfs

    def reload(self):
        return False

    def update_info(self, **weapon):
        args = self._update_info(**weapon)
        if 'clip' in weapon:
            self.clip = weapon['clip']
        if 'clip_size' in weapon:
            self.clip_size = weapon['clip_size']
        GameStateGlobal.weaponList.update(*args)

from game_state import GameStateGlobal
