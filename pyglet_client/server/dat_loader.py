import dats

class DatInterface(object):

    def __init__(self):
        self.index = {}

    def _register_dat(self, name, dat):
        self.index[name] = dat

    def load(self, name, dat):
        klass = self.index[name]
        klass.dat = dat

    def get(self, dat_name, type=None, key=None):
        d = self.index[dat_name]
        if type is not None:
            d = d[type]
            if key is not None:
                d = d[key]
        return d
        
    def get_json(self, dat_name, type=None, key=None):
        d = self.get(dat_name, type, key)
        if type is None:
            d = d.json()
        return d

    def all(self):
        return self.index

    def set_property(self, name, type, key, val):
        klass = self.index[name]
        klass.set(type, key, val)

    def json(self):
        d = {}
        for i, n in self.index.items():
            d[i] = n.json()
        return d

dat_loader = DatInterface()

class Dat(object):
    def __init__(self, name, dat=None):
        if dat is None:
            dat = {
                0:  {}
            }
        self.dat = dat
        self.name = name
        self._register()

    def _register(self):
        global dat_loader
        dat_loader._register_dat(self.name, self)
        
    def get(self, type, prop=None):
        def _get(type, prop):
            if prop is None:
                return self.dat[type]
            else:
                return self.dat[type][prop]
        try:
            d = _get(type, prop)
        except KeyError:
            d = _get(0, prop)   # default
        return d

    def set(self, type, prop, val):
        self.dat[type][prop] = val

    def apply(self, obj):
        for prop in self.dat[0].keys():
            setattr(obj, prop, self.get(obj.type, prop))

    def json(self):
        return self.dat

w_dat = Dat('weapons', dats.weapons)
i_dat = Dat('items', dats.items)
p_dat = Dat('projectiles', dats.projectiles)
c_dat = Dat('cubes', dats.cubes)
