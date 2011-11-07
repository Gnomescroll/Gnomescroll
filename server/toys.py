'''
Toys, miscellaneous objects
'''

import c_lib.terrain_map as terrain_map

from random import randint as rand
from game_objects import DetachableObject, StaticObject, TeamItem
from utils import filter_props

def rand_spot(n=1, lowest=True):
    x,y = rand(5, 15), rand(5, 15)
    if lowest:
        z = terrain_map.get_lowest_open_block(x,y,n)
    else:
        z = terrain_map.get_highest_open_block(x,y,n)
    if z < 0:
        z = terrain_map.zmax
    return x,y,z

class Flag(DetachableObject, TeamItem):

    def __init__(self, id, team, *args, **kwargs):
        DetachableObject.__init__(self, id)
        TeamItem.__init__(self, team)
        self.team = team
        self.spawn()

    def _spawn_point(self):
        return rand_spot(n=self.block_height, lowest=False)

    def spawn(self):
        xyz = self._spawn_point()
        self.pos(xyz)
        self.on_ground = True
        self.owner = None

    def take(self, new_owner):
        if TeamItem.can_take(self, new_owner):
            DetachableObject.take(self, new_owner)

    def json(self, properties=None):
        d = DetachableObject.json(self, properties)
        d.update(TeamItem.json(self, properties))
        if properties is not None:
            d.update(filter_props(self, properties))
        return d        


class Base(StaticObject, TeamItem):

    def __init__(self, id, team, *args, **kwargs):
        StaticObject.__init__(self, id)
        TeamItem.__init__(self, team)
        self.spawned = False
        self.team = team
        self.spawn()
        self.block_height = 2
        
    def spawn(self):
        if not self.spawned:
            self.state[0:3] = rand_spot()
            self.spawned = True

    def agent_nearby(self, agent):
        if agent.team != self.team:
            return
        agent.restore_health_and_ammo()

        if GameStateGlobal.game.mode == 'ctf':
            flags = agent.has_flags()
            for flag in flags:
                print 'agent returning flag'
                agent.drop_item(flag)
                flag.spawn()
                NetOut.event.item_update(flag, properties='state')
                GameStateGlobal.game.score_flag(agent)

    def json(self, properties=None):
        if properties is None:
            d = StaticObject.json(self)
            d.update({
                'team'  :   self.team.id,
            })
        else:
            d = filter_props(self, properties)
        return d

from net_out import NetOut
from game_state import GameStateGlobal
