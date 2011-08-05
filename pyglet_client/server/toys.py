'''
Toys, miscellaneous objects
'''

from game_objects import DetachableObject, StaticObject, filter_props

from random import randint as rand

def rand_spot():
    return (rand(5, 15), rand(5, 15), 15)

class Flag(DetachableObject):

    def __init__(self, id, radius, team, *args, **kwargs):
        DetachableObject.__init__(self, id, radius)
        self.auto_grab = True
        self.drop_on_death = True
        self.team = team
        self.type = 1
        self.spawn()

    def spawn(self):
        xyz = rand_spot()
        self.pos(xyz)
        self.on_ground = True
        self.owner = None

    def json(self, properties=None):
        if properties is None:
            d = DetachableObject.json(self)
            d.update({
                'team'     :   self.team.id,
            })
        else:
            d = filter_props(obj, properties)
        return d        


class Base(StaticObject):

    def __init__(self, id, team, *args, **kwargs):
        StaticObject.__init__(self, id)
        self.spawned = False
        self.team = team
        self.type = 2
        
    def spawn(self):
        if not self.spawned:
            self.state[0:3] = rand_spot()
            self.spawned = True

    def json(self, properties=None):
        if properties is None:
            d = StaticObject.json(self)
            d.update({
                'team'  :   self.team.id,
            })
        else:
            d = filter_props(obj, properties)
        return d
