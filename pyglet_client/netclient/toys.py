'''
Toys, miscellaneous objects
'''

from game_objects import DetachableObject, GameObjectRender, StaticObject
from random import randint as rand

def rand_spot():
    return (rand(0, 100), rand(0, 100), 10)

class Flag(DetachableObject):

    def __init__(self, radius, team):
        DetachableObject.__init__(self, radius, model='flag.vox')
        self.auto_grab = True
        self.drop_on_death = True
        self.team = team
        self.spawn()
        self.type = 1

    def spawn(self):
        xyz = rand_spot()
        self.pos(xyz)
        self.on_ground = True
        self.owner = None


class Base(StaticObject, GameObjectRender):

    def __init__(self, team):
        StaticObject.__init__(self)
        GameObjectRender.__init__(self, 'auto.vox')
        self.spawned = False
        self.team = team
        self.type = 2
        
    def spawn(self):
        if not self.spawned:
            self.state[0:3] = rand_spot()
            self.spawned = True
