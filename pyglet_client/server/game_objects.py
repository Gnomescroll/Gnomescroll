from object_lists import GenericMultiObjectList
from utils import filter_props

object_names = {
    0   :   'Generic',
    1   :   'Flag',
    2   :   'Base',
}


class GameObject:

    def __init__(self, id, state=None):
        self.id = id
        self.on_ground = True
        self.state = state
        if state is None:
            self.state = [0 for i in range(9)]
        self.type = 0
        self.owner = None
        self._set_name()

    def _set_name(self):
        self.name = object_names[self.type]

    def json(self, properties=None):
        d = {
            'id'    :   self.id,
            'type'  :   self.type,
        }
        if properties is None:
            d.update({
                'state' :   self.state,
                'owner' :   self.owner,
                'on_ground':int(self.on_ground),
            })
        else:
            d.update(filter_props(self, properties))

        if 'owner' in d:
            d_own = d['owner']
            if d_own is None:
                d_own = 0
            else:
                d_own = d_own.id
            d['owner'] = d_own
            
        return d

    def pos(self, xyz=None):
        if xyz is not None:
            self.state[0:3] = xyz
        else:
            return self.state[0:3]

    @classmethod
    def name_from_type(self, type):
        return object_names[type]


class StaticObject(GameObject):

    def __init__(self, id, state=None):
        GameObject.__init__(self, id, state)
        self.immobile = True
        
    def pos(self):
        return GameObject.pos(self)


class EquippableObject(GameObject):

    def __init__(self, id, state=None):
        GameObject.__init__(self, id, state)


# pick up / drop
class DetachableObject(GameObject):
    
    def __init__(self, id, radius=1, state=None):
        GameObject.__init__(self, id, state)
        self.radius = radius
        self.auto_grab = False
        self.drop_on_death = True

    def take(self, new_owner):
        # ground -> owner
        old_owner = self.owner
        self.owner = new_owner
        if self.owner is not None:
            self.on_ground = False
        if old_owner != self.owner:
            NetOut.event.item_update(self)

    # give it a position to override default drop position behavior (drop where owner was)
    def drop(self):
        pos = self.owner.pos()
        self.owner = None
        self.pos(pos)
        self.on_ground = True
        NetOut.event.item_update(self)

    def pos(self, xyz=None):
        if self.owner is None:
            return GameObject.pos(self, xyz)
        else:
            if xyz is not None:
                print 'WARNING trying to set item position while item is possessed'
            return self.owner.pos()


class TeamItem:

    def __init__(self, team, own=True, other=True):
        self.team = team
        self.pickup_by_own_team = own
        self.pickup_by_other_team = other

    def can_take(self, new_owner):
        #print new_owner.team, self.team
        return (new_owner.team == self.team and self.pickup_by_own_team) or \
                (new_owner.team != self.team and self.pickup_by_other_team)

    def json(self, properties=None):
        if properties is None:
            d = {
                'team'  :   self.team
            }
        else:
            d = filter_props(self, properties)
        if 'team' in d:
            d['team'] = self.team.id
        return d


class ItemList(GenericMultiObjectList):

    def __init__(self):
        from toys import Flag, Base
        GenericMultiObjectList.__init__(self)
        self._allow_klasses([\
            Flag,
            Base,
        ])

from net_out import NetOut
