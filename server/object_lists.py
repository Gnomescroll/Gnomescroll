# generic game object datastore
# has dictionary interface for retrieving items
class GenericObjectList:

    def __init__(self):
        self.objects = {}
        self._object_type = None
        self._wrapper = None

    def __getitem__(self, key):
        if not key in self.objects:
            print '%s: %s does not exist: id= %s' % (self.__class__.__name__, self._object_type.__name__, str(key),)
        return self.objects[key]

    def __setitem__(self, key, value):
        self.objects[key] = value

    def __delitem__(self, key):
        del self.objects[key]

    def __len__(self):
        return len(self.objects)

    def __contains__(self, key):
        return key in self.objects

    def __iter__(self):
        return iter(self.objects)

    def keys(self):
        return self.objects.keys()

    def values(self):
        return self.objects.values()

    def items(self):
        return self.objects.items()

    def __str__(self):
        return str(self.objects)

    def __repr__(self):
        return repr(self.objects)

    def _add(self, *args, **kwargs):
        object = self._object_type(*args, **kwargs)
        self.objects[object.id] = object
       # print '%s: %s created; id= %s' % (self.__class__.__name__, self._object_type.__name__, object.id,)
        return object

    def _remove(self, obj):
        id = obj.id
        if id in self.objects:
            del self.objects[id]
        #    print '%s: %s removed; id= %s' % (self.__class__.__name__, self._object_type.__name__, id,)
            if self._wrapper is not None:
                self._wrapper.remove(id)
            return True
        return False

    def json(self):
        objects = []
        for obj in self.objects.values():
            objects.append(obj.json())
        return objects

# for tracking multiple objects sharing the same unique identifier counts
## Differences from GenericObjectList:
##  Multiple types of objects can be tracked.
##  They must all share the same unique id counter.
##  Objects used in GenericObjectList make their own ID increment calls in instantiation
##  Objects used in this class do not make their own ID. They are assigned an id by the controller list.
class GenericMultiObjectList(GenericObjectList):

    def __init__(self):
        GenericObjectList.__init__(self)
        self.klass_index = {}               # __name__ -> Klass
        self.klass_registers = {}           # __name__ -> [id, ...]
        self._id = 0

    def _generate_id(self):
        self._id += 1
        return self._id

    def _allow_klass(self, klass):          # use in inheriting class's __init__ to configure objects to be tracked
        self.klass_index[klass.__name__] = klass
        self.klass_registers[klass.__name__] = []

    def _allow_klasses(self, klasses):
        for klass in klasses:
            self._allow_klass(klass)

    def _add(self, klass_name, *args, **kwargs):
        self._object_type = self.klass_index[klass_name]
        id = self._generate_id()
        obj = GenericObjectList._add(self, id, *args, **kwargs)
        self.klass_registers[klass_name].append(obj.id)
        self._object_type = None
        return obj

    def create(self, klass_name, *args, **kwargs):
        return self._add(klass_name, *args, **kwargs)

    def _remove(self, obj):
        klass_name = obj.__class__.__name__
        self.klass_registers[klass_name].remove(obj.id)
        return GenericObjectList._remove(self, obj)

    def destroy(self, obj):
        return self._remove(obj)

    def _filter_klass(self, klass_name):
        klass_ids = self.klass_registers[klass_name]
        objs = [self[kid] for kid in klass_ids]
        return dict(zip(klass_ids, objs))
