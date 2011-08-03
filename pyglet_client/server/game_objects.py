object_names = {
    1   :   'Flag',
    2   :   'Base',
}
    

class GameObject:

    def __init__(self):
        self.on_ground = True
        self.state = [0 for i in range(9)]

    def json(self):
        return {
            'id'    :   self.id,
        }

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
        self.owner = new_owner
        self.on_ground = False
        
    def drop(self):
        self.pos(self.owner.pos())
        self.owner = None
        self.on_ground = True
        

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
