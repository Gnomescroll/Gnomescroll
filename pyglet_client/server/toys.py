'''
Toys, miscellaneous objects
'''

from game_objects import DetachableObject, StaticObject, TeamItem
from utils import filter_props

from random import randint as rand

def rand_spot(z=15):
    return (rand(5, 15), rand(5, 15), z)

class Flag(DetachableObject, TeamItem):

    def __init__(self, id, radius, team, *args, **kwargs):
        DetachableObject.__init__(self, id, radius)
        TeamItem.__init__(self, team, own=False, other=True)
        self.auto_grab = True
        self.drop_on_death = True
        self.team = team
        self.type = 1
        self._set_name()
        self.spawn()

    def spawn(self):
        xyz = rand_spot()
        self.pos(xyz)
        self.on_ground = True
        self.owner = None

    def take(self, new_owner):
        if TeamItem.can_take(self, new_owner):
            DetachableObject.take(self, new_owner)

    def json(self, properties=None):
        if properties is None:
            d = DetachableObject.json(self)
            d.update({
                'team'     :   self.team.id,
            })
        else:
            d = filter_props(self, properties)
        return d        


class Base(StaticObject, TeamItem):

    def __init__(self, id, team, *args, **kwargs):
        StaticObject.__init__(self, id)
        TeamItem.__init__(self, team, False, False)
        self.spawned = False
        self.team = team
        self.type = 2
        self._set_name()
        self.spawn()
        self.radius = 2
        
    def spawn(self):
        if not self.spawned:
            self.state[0:3] = rand_spot(z=2)
            self.spawned = True

    def agent_nearby(self, agent):
        if agent.team != self.team:
            return
        agent.restore_health_and_ammo()
        flag = agent.has_flag()
        if flag:
            print 'agent returned flag'
            agent.drop_item(flag)
            flag.spawn()

    def json(self, properties=None):
        if properties is None:
            d = StaticObject.json(self)
            d.update({
                'team'  :   self.team.id,
            })
        else:
            d = filter_props(self, properties)
        return d
