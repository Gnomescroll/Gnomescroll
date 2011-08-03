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

    def __init__(self):
        self.on_ground = True
        self.state = [0 for i in range(9)]

    def json(self, properties=None):
        if properties is None:
            return {
                'id'    :   self.id,
                'state' :   self.state,
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

    def __init__(self):
        GameObject.__init__(self)
        self.immobile = True
        
    def pos(self):
        return GameObject.pos(self)


class EquippableObject(GameObject):

    def __init__(self):
        GameObject.__init__(self)


# pick up / drop
class DetachableObject(GameObject):
    
    def __init__(self, radius=1):
        GameObject.__init__(self)
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
            NetOut.event.object_update(self)
        
    def drop(self):
        self.pos(self.owner.pos())
        self.owner = None
        self.on_ground = True
        NetOut.event.object_update(self)

    def json(self, properties=None):
        if properties is None:
            d = self.json()
            d.update({
                'owner'     :   self.owner,
                'on_ground' :   int(self.on_ground)
            }
        else:
            d = filter_props(obj, properties)
        return d
            

class ObjectList(GenericMultiObjectList):

    def __init__(self):
        from toys import Flag, Base
        GenericMultiObjectList.__init__(self)
        self._allow_klasses([\
            Flag,
            Base,
        ])

    def destroy(self, obj):
        return self._remove(self, obj)

    def create(self, klass_name=None, *args, **kwargs):
        if klass_name is None:
            if 'name' in kwargs:
                klass_name = kwargs['name']
            elif 'type' in kwargs:
                klass_name = GameObject.name_from_type(kwargs['type'])
        else:
            klass_name = GameObject.name_from_type(klass_name)
            
        return self._add(klass_name, *args, **kwargs)

    def update(self, obj, id=None):
        if id is None:
            return
        old = self[id]

        if old.id != obj.id and old.id in self.objects:
            del self.objects[old.id]
        self.objects[obj.id] = obj
