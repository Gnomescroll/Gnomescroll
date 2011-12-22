'''
Toys, miscellaneous objects
'''

from game_objects import DetachableObject, StaticObject, TeamItem

class Flag(DetachableObject, TeamItem):

    def __init__(self, id, team=None, state=None, **kwargs):
        DetachableObject.__init__(self, id, state=state, **kwargs)
        TeamItem.__init__(self, team, **kwargs)

    def take(self, new_owner):
        if TeamItem.can_take(self, new_owner):
            taken = DetachableObject.take(self, new_owner)
        
class Base(StaticObject, TeamItem):

    def __init__(self, id, team=None, state=None, **kwargs):
        StaticObject.__init__(self, id, state, **kwargs)
        TeamItem.__init__(self, team, **kwargs)

from game_state import GameStateGlobal
