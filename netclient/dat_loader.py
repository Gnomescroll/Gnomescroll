'''
Stores server game dats
'''

class DatInterface(object):

    def __init__(self):
        self.index = {}
        self.loaded = {}
        self.fully_loaded = False

    def _register_dat(self, name, dat):
        self.index[name] = dat
        self.loaded[name] = False

    def check_loaded(self):
        for rdy in self.loaded:
            if not rdy:
                return
        self.fully_loaded = True

    # loads a dat by name
    def load(self, name, dat):
        klass = self.index[name]
        klass.update(dat)
        self.loaded[name] = True
        self.check_loaded()

    #  loads a collection of named dats
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
        #callbacks
        self.on_change = lambda: None
        self.on_first_load = lambda: None
        self.loaded_once = False

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
        # callbacks
        if not self.loaded_once:
            self.loaded_once = True
            self.on_first_load()
        self.on_change()

    def set(self, type, prop, val):
        self.dat[type][prop] = val
        self.on_change(type)

    def apply(self, obj):
        for prop in self.dat[0].keys():
            setattr(obj, prop, self.get(obj.type, prop))

w_dat = Dat('weapons')
i_dat = Dat('items')
#p_dat = Dat('projectiles')
c_dat = Dat('cubes')
