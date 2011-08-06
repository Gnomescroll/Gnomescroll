'''
Toys, miscellaneous objects
'''

from game_objects import DetachableObject, StaticObject, TeamItem
from vox import GameObjectRender, TeamItemRender
#from random import randint as rand

#def rand_spot():
    #return (rand(0, 100), rand(0, 100), 10)

class Flag(DetachableObject, TeamItem, TeamItemRender):

    def __init__(self, id, radius=None, team=None, state=None, **kwargs):
        if team is not None:
            team = GameStateGlobal.teamList[team]
        DetachableObject.__init__(self, id, radius, state=state)
        TeamItem.__init__(self, team, own=False, other=True)
        TeamItemRender.__init__(self, model='black_flag.vox')
        self.auto_grab = True
        self.drop_on_death = True
        #self.spawn()
        self.type = 1
        self.radius = 1

    #def spawn(self):
        #xyz = rand_spot()
        #self.pos(xyz)
        #self.on_ground = True
        #self.owner = None


class Base(StaticObject, GameObjectRender, TeamItem):

    def __init__(self, id, radius=None, team=None, state=None, **kwargs):
        if team is not None:
            team = GameStateGlobal.teamList[team]
        StaticObject.__init__(self, id, state)
        TeamItem.__init__(self, team, False, False)
        GameObjectRender.__init__(self, 'auto.vox')
        #self.spawned = False
        self.type = 2
        self.radius = 1
        
    #def spawn(self):
        #if not self.spawned:
            #self.state[0:3] = rand_spot()
            #self.spawned = True

from game_state import GameStateGlobal
