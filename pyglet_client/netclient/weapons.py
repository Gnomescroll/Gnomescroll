from game_objects import GameObject
from game_objects import EquippableObject
import animations

weapon_dat = {
    0:  {
        'projectile_type'   :   0
    },
    1:  {
        'projectile_type'   :   1
    },
    2:  {
        'projectile_type'   :   0
    },
    3:  {
        'projectile_type'   :   0
    },
    4:  {
        'projectile_type'   :   1
    },
}

class Weapon(EquippableObject):

    _weapons = {
        'Weapon'    :   0,
        'LaserGun'  :   1,
        'Pick'      :   2,
        'BlockApplier':   3,
        'HitscanLaserGun': 4,
    }

    def __init__(self, id, owner=None, state=None):
        EquippableObject.__init__(self, id, state)
        self.owner = owner
        self.type = self._weapons[self.__class__.__name__]
        self.hitscan = False
        self.ptype = weapon_dat[self.type]['projectile_type']  # projectile type, implement fully later
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
        undef = '--'
        ammo = getattr(self, 'ammo', undef)
        max_ammo = getattr(self, 'max_ammo', undef)
        clip = getattr(self, 'clip', undef)
        clip_size = getattr(self, 'clip_size', undef)
        strfs = tuple([str(a) for a in [clip, clip_size, ammo, max_ammo]])
        return '%s/%s::%s/%s' % strfs

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
        GameStateGlobal.weaponList.update(*args)

class Pick(Weapon):

    def __init__(self, id, owner=None, state=None, **kwargs):
        Weapon.__init__(self, id, owner, state)

    def fire(self):
        return 'hit_block'

    def reload(self):
        return False

from game_state import GameStateGlobal
