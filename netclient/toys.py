'''
Toys, miscellaneous objects
'''

from game_objects import DetachableObject, StaticObject, TeamItem
from vox import GameObjectRender, TeamItemRender

class Flag(DetachableObject, TeamItem, TeamItemRender):

    def __init__(self, id, team=None, state=None, **kwargs):
        DetachableObject.__init__(self, id, state=state, **kwargs)
        TeamItem.__init__(self, team, **kwargs)

    def take(self, new_owner):
        if TeamItem.can_take(self, new_owner):
            taken = DetachableObject.take(self, new_owner)
        self.vox.visibile = self.on_ground
        
class Base(StaticObject, TeamItemRender, TeamItem):

    def __init__(self, id, team=None, state=None, **kwargs):
        StaticObject.__init__(self, id, state, **kwargs)
        TeamItem.__init__(self, team, **kwargs)

from game_state import GameStateGlobal
