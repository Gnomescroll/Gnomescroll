from object_lists import GenericMultiObjectList
from utils import filter_props

import dats.loader as dat_loader
i_dat = dat_loader.i_dat

class GameObject:

    _types = {
        'GameObject'    :   0,
        'StaticObject'  :   1,
        'EquippableObject': 2,
        'DetachableObject': 3,
        'Flag'          :   4,
        'Base'          :   5,
    }

    dat = i_dat

    def __init__(self, id, state=None):
        self.id = id
        self.on_ground = True
        self.owner = None
        if state is None:
            state = [0 for i in range(9)]
        self.state = state
        self._set_type()
        self.block_height = 1
        self.dat.apply(self)


    def _set_type(self):
        self.type = self._types[self.__class__.__name__]

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
            if self.on_ground:
                self.last_ground_pos = xyz
        else:
            return self.state[0:3]


class StaticObject(GameObject):

    def __init__(self, id, state=None):
        GameObject.__init__(self, id, state)
        
    def pos(self):
        return GameObject.pos(self)


class EquippableObject(GameObject):

    def __init__(self, id, state=None):
        GameObject.__init__(self, id, state)


# pick up / drop
class DetachableObject(GameObject):
    
    def __init__(self, id, state=None):
        GameObject.__init__(self, id, state)
        self.last_ground_pos = self.state[0:3]

    def take(self, new_owner):
        if self.on_ground:
            self.last_ground_pos = self.pos()
        # ground -> owner
        old_owner = self.owner
        self.owner = new_owner
        if self.owner is not None:
            self.on_ground = False
        if old_owner != self.owner:
            NetOut.event.item_update(self)
        print 'taking %s' % (self,)
        print '%s --> %s' % (old_owner, self.owner,)

    # give it a position to override default drop position behavior (drop where owner was)
    def drop(self):
        if self.owner is not None:
            pos = self.owner.pos()
        else:
            pos = self.last_ground_pos
            print 'Error: attempting to drop item that has no owner'
        self.owner = None
        self.pos(pos)
        self.on_ground = True
        NetOut.event.item_update(self)

    def can_take(self, new_owner):
        if self.on_ground and self.owner != new_owner:
            return True
        return False

    def can_drop(self, owner):
        if owner is not None and self.owner == owner:
            return True
        return False

    def pos(self, xyz=None):
        if self.owner is None:
            return GameObject.pos(self, xyz)
        else:
            if xyz is not None:
                print 'WARNING trying to set item position while item is possessed'
            return self.owner.pos()

from net_out import NetOut
