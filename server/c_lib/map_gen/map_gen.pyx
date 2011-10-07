cdef extern from "./map_gen/interpolator.h":
    void apply_interp1(int x, int ix)
    void apply_interp2(int x, int y, int ix, int iy)
    void apply_interp3(int x, int y, int z, int ix, int iy, int iz)

cdef extern from "./map_gen/gradient.h":
    void apply_grad1(int x,               float x0, float x1)
    void apply_grad2(int x, int y,        float x0, float x1, float y0, float y1)
    void apply_grad3(int x, int y, int z, float x0, float x1, float y0, float y1,
                                                              float z0, float z1)
cdef extern from "./map_gen/noise.h":
    void clear_noisemap()
    void set_terrain_density(int x, int y, int z)
    void set_terrain_height(int x, int y, int z, int baseline, int maxheight)

cdef extern from "./map_gen/features.h":
    void grass()


from c_lib.noise import Simplex, Perlin, RMF, set_seed

import time

class Config:

    def __init__(self):
        self.x = self.y = self.z = 0

        self.interp = False
        self.ix = self.iy = self.iz = 1

        self.grad = False
        self.gx0 = self.gx1 = self.gy0 = self.gy1 = self.gz0 = self.gz1 = 0.0

        self.octaves = 1
        self.persistence = 0.5
        self.frequency = 1.0
        self.amplitude = 1.0
        self.repeatx = 1024
        self.repeaty = 1024
        self.repeatz = 1024
        self.base = 0

        self.rmf = False
        self.dim = 2
        self.noise_type = ''    # 'p' for perlin, 's' for simplex
        self.noise = None

        self.use_grass = False

    def reset(self):
        self.__init__()
        return self

    def size(self, x,y,z):
        self.x = int(x)
        self.y = int(y)
        self.z = int(z)
        return self

    def interpolate(self, x,y,z):
        self.interp = True
        self.ix = int(x)
        self.iy = int(y)
        self.iz = int(z)
        return self

    def heightmap(self, baseline=0, maxheight=0):
        self.baseline = baseline
        self.maxheight = maxheight
        return self
        
    def gradient(self, x0=0.0, x1=0.0, y0=0.0, y1=0.0, z0=0.0, z1=0.0):
        self.grad = True
        self.gx0 = float(x0)
        self.gx1 = float(x1)
        self.gy0 = float(y0)
        self.gy1 = float(y1)
        self.gz0 = float(z0)
        self.gz1 = float(z1)
        return self

    def p1(self):
        self.dim = 1
        self.noise_type = 'p'
        return self

    def p2(self):
        self.dim = 2
        self.noise_type = 'p'
        return self
        
    def p3(self):
        self.dim = 3
        self.noise_type = 'p'
        return self

    def s2(self):
        self.dim = 2
        self.noise_type = 's'
        return self

    def s3(self):
        self.dim = 3
        self.noise_type = 's'
        return self

    def rmf(self):
        self.rmf = True
        return self

    def seed(self, s):
        set_seed(s)
        return self

    def grass(self):
        self.use_grass = True
        return self

    def start(self):
        _n = time.time()
        
        noise_method = 'noise%d' % (self.dim,)
        if self.rmf:
            self.noise = RMF(octaves=self.octaves,
                             persistence=self.persistence,
                             frequency=self.frequency,
                             amplitude=self.amplitude,
                             repeatx=self.repeatx,
                             repeaty=self.repeaty,
                             repeatz=self.repeatz,
                             base=self.base)
            noise_method = self.noise_type + noise_method
        elif self.noise_type == 'p':
            self.noise = Perlin(octaves=self.octaves,
                                persistence=self.persistence,
                                frequency=self.frequency,
                                amplitude=self.amplitude,
                                repeatx=self.repeatx,
                                repeaty=self.repeaty,
                                repeatz=self.repeatz,
                                base=self.base)
        elif self.noise_type == 's':
            self.noise = Simplex(octaves=self.octaves,
                                 persistence=self.persistence,
                                 frequency=self.frequency,
                                 amplitude=self.amplitude)

        size_args = [self.x, self.y, self.z][:self.dim]
        interp_args = [self.iz, self.iy, self.iz][:self.dim]
        #grad_args = [self.gx0, self.gx1, self.gy0, self.gy1, self.gz0, self.gz1][:self.dim*2]
        grad_args = [self.gx0, self.gx1, self.gy0, self.gy1, self.gz0, self.gz1]

        if self.noise is not None:
            self.noise.fill()
            if self.interp:
                interpolates[self.dim](*(size_args+interp_args))
            else:
                getattr(self.noise, noise_method)(*size_args)
            
        if self.grad:
            print 'grad'
            #gradients[self.dim](*(size_args+grad_args))
            apply_gradient3(self.x, self.y, self.z, self.gx0, self.gx1, self.gy0, self.gy1, self.gz0, self.gz1)

        if self.dim == 1:
            print "Dimension 1 terrain not implemented."
        elif self.dim == 2:
            print "setting terrain height"
            set_terrain_height(self.x, self.y, self.z, self.baseline, self.maxheight)
        elif self.dim == 3:
            set_terrain_density(self.x, self.y, self.z)

        # features
        if self.use_grass:
            grass()

        print 'map gen took %0.2f seconds' % (time.time() - _n)


        
conf = Config()

def reset():
    clear_noisemap()

def apply_gradient1(x, x0, x1):
    apply_grad1(x, x0,x1)

def apply_gradient2(x,y, x0, x1, y0, y1):
    apply_grad2(x,y, x0, x1, y0, y1)

def apply_gradient3(x,y,z, x0, x1, y0, y1, z0, z1):
    print "apply grad3"
    apply_grad3(x,y,z,  x0, x1, y0, y1, z0, z1)


gradients = {
    1: apply_gradient1,
    2: apply_gradient2,
    3: apply_gradient3,
}

def apply_interpolate1(x, ix):
    apply_interp1(x, ix)
    
def apply_interpolate2(x,y, ix,iy):
    apply_interp2(x,y, ix,iy)
    
def apply_interpolate3(x,y,z, ix,iy,iz):
    apply_interp3(x,y,z, ix,iy,iz)

interpolates = {
    1: apply_interpolate1,
    2: apply_interpolate2,
    3: apply_interpolate3,
}

def generate():
    conf.start()
