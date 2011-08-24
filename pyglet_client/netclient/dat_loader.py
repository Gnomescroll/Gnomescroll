'''
Stores server game dats
'''

class DatInterface(object):

    def __init__(self):
        self.index = {}

    def _register_dat(self, name, dat):
        self.index[name] = dat

    def load(self, name, dat):
        klass = self.index[name]
        klass.update(dat)

    def load_all(self, dat):
        for name, d in dat.items():
            self.load(name, d)

    def set_property(self, name, type, key, val):
        klass = self.index[name]
        klass.set(type, key, val)

dat_loader = DatInterface()

class Dat(object):
    def __init__(self, name):
        self.dat = {
            0 : {}
        }
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

    def update(self, dat):
        self.dat.update(dat)
        for type, props in self.dat.items():
            del self.dat[type]
            self.dat[int(type)] = props

    def set(self, type, prop, val):
        self.dat[type][prop] = val

    def apply(self, obj):
        for prop in self.dat[0].keys():
            setattr(obj, prop, self.get(obj.type, prop))

w_dat = Dat('weapons')
i_dat = Dat('items')
p_dat = Dat('projectiles')

