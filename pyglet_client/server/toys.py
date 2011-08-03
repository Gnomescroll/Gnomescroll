'''
Toys, miscellaneous objects
'''

from game_objects import DetachableObject, StaticObject, filter_props

from random import randint as rand

def rand_spot():
    return (rand(0, 100), rand(0, 100), 10)

class Flag(DetachableObject):

    def __init__(self, radius, team):
        DetachableObject.__init__(self, radius)
        self.auto_grab = True
        self.drop_on_death = True
        self.team = team
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
            }
        else:
            d = filter_props(obj, properties)
        return d        


class Base(StaticObject):

    def __init__(self, team):
        StaticObject.__init__(self)
        self.spawned = False
        self.team = team
        
    def spawn(self):
        if not self.spawned:
            self.state[0:3] = rand_spot()
            self.spawned = True
