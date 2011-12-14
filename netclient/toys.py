'''
Toys, miscellaneous objects
'''

from game_objects import DetachableObject, StaticObject, TeamItem
from vox import GameObjectRender, TeamItemRender

class Flag(DetachableObject, TeamItem, TeamItemRender):

    def __init__(self, id, team=None, state=None, **kwargs):
        if team is not None:
            team = GameStateGlobal.teamList[team]
        DetachableObject.__init__(self, id, state=state, **kwargs)
        TeamItem.__init__(self, team, **kwargs)
        #TeamItemRender.__init__(self, self, model='black_flag.vox')

    def take(self, new_owner):
        if TeamItem.can_take(self, new_owner):
            taken = DetachableObject.take(self, new_owner)
        self.vox.visibile = self.on_ground
        
class Base(StaticObject, TeamItemRender, TeamItem):

    def __init__(self, id, team=None, state=None, **kwargs):
        if team is not None:
            team = GameStateGlobal.teamList[team]
        StaticObject.__init__(self, id, state, **kwargs)
        TeamItem.__init__(self, team, **kwargs)
        #TeamItemRender.__init__(self, self, 'base.vox')

from game_state import GameStateGlobal
