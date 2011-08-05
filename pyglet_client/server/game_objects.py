from object_lists import GenericMultiObjectList


object_names = {
    1   :   'Flag',
    2   :   'Base',
}
    

def filter_props(obj, properties):
    d = {}
    for prop in properties:
        val = getattr(obj, prop, None)
        if val is None:
            continue
        if type(val) == bool:
            val = int(val)
        d[prop] = val
    return d
    

class GameObject:

    def __init__(self, id, state=None):
        self.id = id
        self.on_ground = True
        self.state = state
        if state is None:
            self.state = [0 for i in range(9)]
        self.type = 0
        self.owner = None

    def json(self, properties=None):
        if properties is None:
            return {
                'id'    :   self.id,
                'state' :   self.state,
                'type'  :   self.type,
            }
        else:
            return filter_props(self, properties)

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
        print 'being taken by %s' % (new_owner,)
        old_owner = self.owner
        self.owner = new_owner
        if self.owner is not None:
            self.on_ground = False
        if old_owner != self.owner:
            NetOut.event.item_update(self)
        
    def drop(self):
        self.pos(self.owner.pos())
        self.owner = None
        self.on_ground = True
        NetOut.event.item_update(self)

    def json(self, properties=None):
        if properties is None:
            d = GameObject.json(self)
            if self.owner is not None:
                d['owner'] = self.owner.id
            d['on_ground'] = int(self.on_ground)
        else:
            d = filter_props(obj, properties)
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
