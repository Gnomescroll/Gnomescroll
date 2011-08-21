from game_objects import GameObject
from game_objects import EquippableObject
import animations

class Weapon(EquippableObject):

    _weapons = {
        'Weapon'    :   0,
        'LaserGun'  :   1,
        'Pick'      :   2,
        'BlockApplier':   3,
        'HitscanLaserGun': 4,
        'GrenadePouch'  :   5,
        'GrenadePouch_C':   6,
    }

    _hud_undef = '--'

    def __init__(self, id, owner=None, state=None):
        EquippableObject.__init__(self, id, state)
        self.owner = owner
        self.type = self._weapons[self.__class__.__name__]
        self.hitscan = False
        self._animation = animations.Animation

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
        undef = self._hud_undef
        ammo = getattr(self, 'ammo', undef)
        max_ammo = getattr(self, 'max_ammo', undef)
        clip = getattr(self, 'clip', undef)
        clip_size = getattr(self, 'clip_size', undef)
        #strfs = tuple([str(a) for a in [clip, clip_size, ammo, max_ammo]])
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

class LaserGun(Weapon):

    def __init__(self, id=None, owner=None, clip=None, state=None, **kwargs):
        Weapon.__init__(self, id, owner, state)
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

    def update_info(self, **weapon):
        print 'updating weapon %s' % (weapon,)
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
        GameStateGlobal.weaponList.update(*args)

class HitscanLaserGun(LaserGun):

    def __init__(self, id=None, owner=None, clip=None, state=None, **kwargs):
        LaserGun.__init__(self, id=id, owner=owner, clip=clip, state=state, **kwargs)
        self.hitscan = True
        self._animation = animations.HitscanLaserGunAnimation

    def animation(self, target=None, agent=None, vector=None):
        if agent is None:
            agent = GameStateGlobal.agentList[self.owner]
        origin = agent.pos()
        if vector is None:
            vector = agent.direction()
        anim = self._animation(origin, vector, target)
        return anim
    

class BlockApplier(Weapon):

    def __init__(self, id=None, owner=None, clip=None, state=None, **kwargs):
        Weapon.__init__(self, id, owner, state)
        self.max_ammo = 0
        self.ammo = 0
        self.clip_size = 100
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
        self.max_ammo = 100
        self.ammo = 0
        self.clip_size = 100
        self.clip = self.clip_size

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

class GrenadePouch_C(GrenadePouch):

    def __init__(self, id, owner=None, state=None, **kwargs):
        pass

    def fire(self):
        super(GrenadePouch_C, self).fire()
        return 'throw_grenade_c'

from game_state import GameStateGlobal
