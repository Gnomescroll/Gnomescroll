'''
Toys, miscellaneous objects
'''

from game_objects import DetachableObject, GameObjectRender, StaticObject
#from random import randint as rand

#def rand_spot():
    #return (rand(0, 100), rand(0, 100), 10)

class Flag(DetachableObject):

    def __init__(self, id, radius=None, team=None, state=None, **kwargs):
        DetachableObject.__init__(self, id, radius, state=state, model='flag.vox')
        self.auto_grab = True
        self.drop_on_death = True
        if team is not None:
            self.team = GameStateGlobal.teamList[team]
        else:
            self.team = team
        #self.spawn()
        self.type = 1
        self.radius = 1

    #def spawn(self):
        #xyz = rand_spot()
        #self.pos(xyz)
        #self.on_ground = True
        #self.owner = None


class Base(StaticObject, GameObjectRender):

    def __init__(self, id, radius=None, team=None, state=None, **kwargs):
        StaticObject.__init__(self, id, state)
        GameObjectRender.__init__(self, 'auto.vox')
        #self.spawned = False
        if team is not None:
            self.team = GameStateGlobal.teamList[team]
        else:
            self.team = team
        self.type = 2
        self.radius = 1
        
    #def spawn(self):
        #if not self.spawned:
            #self.state[0:3] = rand_spot()
            #self.spawned = True

from game_state import GameStateGlobal
