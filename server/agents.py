'''
Agents:
    Objects through which a Player experiences the game world
'''
import opts
opts = opts.opts

from game_state import GameStateGlobal
from object_lists import GenericObjectList
from net_out import NetOut

import c_lib.c_lib_agents as cAgents
import c_lib.c_lib_game_modes as cGame

# datastore controller for agents
class AgentList(GenericObjectList):

    def __init__(self):
        GenericObjectList.__init__(self)
        self._object_type = Agent
        self._wrapper = cAgents.AgentListWrapper

    def create(self, client_id):
        return self._add(client_id)

    def destroy(self, agent):
        id = self._remove(agent)
        return id

class Agent(cAgents.AgentWrapper):

    def __init__(self, client_id):
        cAgents.AgentWrapper.__init__(self, client_id)
        cAgents.AgentWrapper.send_id_to_client(self, client_id)
        
        wl = GameStateGlobal.weaponList
        self.weapons = [    \
            wl.create('HitscanLaserGun', owner=self),
            wl.create('Pick', owner=self),
            wl.create('BlockApplier', owner=self),
            wl.create('GrenadePouch', owner=self),
        ]
        self._active_weapon = 0

    def __getattribute__(self, name):
        try:
            val = cAgents.AgentWrapper.__getattribute__(self, name)
        except AttributeError:
            val = object.__getattribute__(self, name)

        if name == 'team':
            val = cGame.get_team(val)

        return val

    @property
    def state(self):
        return [self.x, self.y, self.z, self.vx, self.vy, self.vz, self.ax, self.ay, self.az]
    @state.setter
    def state(self, val):
        self.x, self.y, self.z, self.vx, self.vy, self.vz, self.ax, self.ay, self.az = val

    def json(self, properties=None): # json encodable string representation
        d = {
            'id'    :   self.id,
        }
        if properties is None:
            d.update({
                'weapons': {
                    'weapons': [weapon.json() for weapon in self.weapons],
                    'active' : self._active_weapon,
                },
            })
        else:
            if type(properties) == str:
                properties = [properties]
            for prop in properties:
                if prop == 'weapons':
                    val = {
                        'weapons': [weapon.json() for weapon in self.weapons],
                        'active' : int(self._active_weapon),
                    }
                elif prop == 'active_weapon':
                    prop = 'weapons'
                    val = {
                        'active'    :   int(self._active_weapon),
                    }
                else:
                    val = getattr(self, prop)

                if type(val) == bool:
                    val = int(val)
                d[prop] = val
        return d

    def set_active_weapon(self, weapon_index):
        old = self._active_weapon
        self._active_weapon = weapon_index
        if old != weapon_index:
            NetOut.event.agent_update(self, 'active_weapon')

    def active_weapon(self):
        return self.weapons[self._active_weapon]

    def restore_ammo(self):
        for weapon in self.weapons:
            weapon.refill()
