cdef extern from "./map_gen/interpolator.h":
    int seed_max

    void apply_interp1_perlin(int x, int ix, int rep, int base)
    void apply_interp1_rmf_perlin(int x, int ix, int rep, int base)
    
    void apply_interp2_perlin(int x, int y, int ix, int iy, int rep_x, int rep_y, int base)
    void apply_interp2_rmf_perlin(int x, int y, int ix, int iy, int rep_x, int rep_y, int base)

    void apply_interp2_simplex(int x, int y, int ix, int iy)
    void apply_interp2_rmf_simplex(int x, int y, int ix, int iy)

    void apply_interp3_perlin(int x, int y, int z, int ix, int iy, int iz, int rep_x, int rep_y, int rep_z, int base)
    void apply_interp3_rmf_perlin(int x, int y, int z, int ix, int iy, int iz, int rep_x, int rep_y, int rep_z, int base)

    void apply_interp3_simplex(int x, int y, int z, int ix, int iy, int iz)
    void apply_interp3_rmf_simplex(int x, int y, int z, int ix, int iy, int iz)

cdef extern from "./map_gen/gradient.h":
    void apply_grad1(int x,               float x0, float x1)
    void apply_grad2(int x, int y,        float x0, float x1, float y0, float y1)
    void apply_grad3(int x, int y, int z, float x0, float x1, float y0, float y1,
                                                              float z0, float z1)
cdef extern from "./map_gen/noise.h":
    void clear_noisemap()
    void set_terrain_density(int x, int y, int z, float threshold, int tile)
    void set_terrain_height(int x, int y, int z, int baseline, int maxheight, int tile)
    void invert_map(int x, int y, int z, int tile)
    void set_noise_parameters(int octaves, float persistence, float amplitude, float lacunarity, float frequency)

cdef extern from "./map_gen/features.h":
    void _grass(int x, int y, int base)
    void _caves(int x, int y, int z, int base)

from c_lib.noise import Simplex, Perlin, RMF, set_seed

import time

xmax, ymax, zmax = 512, 512, 128 # cdef extern from tmap later [[have to de-#define]]

class Config:

    seed_int = 1
    seeds = []
    seed_index = 0
    
    def __init__(self):
        self.x = self.y = self.z = 0
        self.base_tile = 2 # stone (1 is error block)

        self.interp = False
        self.ix = self.iy = self.iz = 1

        self.grad = False
        self.gx0 = self.gx1 = self.gy0 = self.gy1 = self.gz0 = self.gz1 = 0.0

        self.octaves = 1
        self.persistence = 0.5
        self.frequency = 1.0
        self.amplitude = 1.0
        self.lacunarity = 2.0
        self.repeatx = 1024
        self.repeaty = 1024
        self.repeatz = 1024
        self.base = 0

        self.dim = 2
        self.noise_type = ''    # 'p' for perlin, 's' for simplex
        self.noise = None

        self.density_threshold = 0.0
        
        self.use_rmf = False
        self.use_heightmap = False
        self.use_density = False

        self.add_grass = False

    def reset(self):
        self.__init__()
        return self

    def size(self, x,y,z):
        self.x = int(x)
        self.y = int(y)
        self.z = int(z)
        return self

    def max_size(self): # uses max size available
        self.x = xmax
        self.y = ymax
        self.z = zmax
        return self

    def tile(self, t):
        self.base_tile = t
        return self

    def interpolate(self, x,y,z):
        self.interp = True
        self.ix = int(x)
        self.iy = int(y)
        self.iz = int(z)
        return self

    def heightmap(self, baseline=0, maxheight=0):
        self.use_heightmap = True
        if baseline + maxheight > zmax:
            print "Map_gen heightmap settings: baseline + maxheight exceed map height. Scaling down"
            scale = float(zmax) / float(baseline + maxheight)
            baseline = int(baseline * scale)
            maxheight = int(maxheight * scale)
            params = [baseline, maxheight]
            i = 0
            while sum(params) < zmax:
                params[i] += 1
                i += 1
                i %= 2
            baseline, maxheight = params
            
        self.baseline = baseline
        self.maxheight = maxheight
        return self

    def density(self, threshold=0.0):
        self.use_density = True
        self.density_threshold = threshold
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

    def _noise_conf(self, octaves, persistence, amplitude, frequency, lacunarity):
        if octaves is not None:
            self.octaves = octaves
        if persistence is not None:
            self.persistence = persistence
        if amplitude is not None:
            self.amplitude = amplitude
        if frequency is not None:
            self.frequency = frequency
        if lacunarity is not None:
            self.lacunarity = lacunarity
        return self

    def p1(self, octaves=None, persistence=None, amplitude=None, frequency=None, lacunarity=None):
        self.dim = 1
        self.noise_type = 'p'
        self._noise_conf(octaves, persistence, amplitude, frequency, lacunarity)
        return self

    def p2(self, octaves=None, persistence=None, amplitude=None, frequency=None, lacunarity=None):
        self.dim = 2
        self.noise_type = 'p'
        self._noise_conf(octaves, persistence, amplitude, frequency, lacunarity)
        return self
        
    def p3(self, octaves=None, persistence=None, amplitude=None, frequency=None, lacunarity=None):
        self.dim = 3
        self.noise_type = 'p'
        self._noise_conf(octaves, persistence, amplitude, frequency, lacunarity)
        return self

    def s2(self, octaves=None, persistence=None, amplitude=None, frequency=None, lacunarity=None):
        self.dim = 2
        self.noise_type = 's'
        self._noise_conf(octaves, persistence, amplitude, frequency, lacunarity)
        return self

    def s3(self, octaves=None, persistence=None, amplitude=None, frequency=None, lacunarity=None):
        self.dim = 3
        self.noise_type = 's'
        self._noise_conf(octaves, persistence, amplitude, frequency, lacunarity)
        return self

    def rmf(self):
        self.use_rmf = True
        return self

    def _seeds(self):
        self.seeds = []
        n = self.seed_int
        for i in range(10):
            n += n * i
            n %= seed_max
            self.seeds.append(n)

    def seed(self, s):
        self.seed_int = s % seed_max
        self._seeds()
        set_seed(s)
        return self

    def get_current_seed(self):
        return self.seeds[self.seed_index]

    def grass(self):
        self.add_grass = True
        return self

    def start(self):
        _n = time.time()

        set_noise_parameters(self.octaves, self.persistence, self.amplitude, self.lacunarity, self.frequency)
        
        noise_method = 'noise%d' % (self.dim,)
        if self.use_rmf:
            self.noise = RMF(octaves=self.octaves,
                             persistence=self.persistence,
                             amplitude=self.amplitude,
                             lacunarity = self.lacunarity,
                             frequency=self.frequency,
                             repeatx=self.repeatx,
                             repeaty=self.repeaty,
                             repeatz=self.repeatz,
                             base=self.base)
            noise_method = self.noise_type + noise_method
        elif self.noise_type == 'p':
            self.noise = Perlin(octaves=self.octaves,
                                persistence=self.persistence,
                                amplitude=self.amplitude,
                                lacunarity=self.lacunarity,
                                frequency=self.frequency,
                                repeatx=self.repeatx,
                                repeaty=self.repeaty,
                                repeatz=self.repeatz,
                                base=self.base)
        elif self.noise_type == 's':
            self.noise = Simplex(octaves=self.octaves,
                                 persistence=self.persistence,
                                 amplitude=self.amplitude,
                                 lacunarity=self.lacunarity,
                                 frequency=self.frequency)

        size_args = [self.x, self.y, self.z][:self.dim]
        interp_args = [self.iz, self.iy, self.iz][:self.dim]
        #grad_args = [self.gx0, self.gx1, self.gy0, self.gy1, self.gz0, self.gz1][:self.dim*2]
        grad_args = [self.gx0, self.gx1, self.gy0, self.gy1, self.gz0, self.gz1]

        noise_args = []
        if self.noise_type == 'p':
            noise_args += [self.repeatx, self.repeaty, self.repeatz][:self.dim]
            noise_args.append(self.base)

        
        if self.noise is not None:
            self.noise.fill()
            if self.interp:
                interpolates[self.dim](noise_type=self.noise_type, rmf=self.use_rmf, *(size_args+interp_args+noise_args))
            else:
                getattr(self.noise, noise_method)(*size_args)
            
        if self.grad:
            print 'grad'
            #gradients[self.dim](*(size_args+grad_args))
            apply_gradient3(self.x, self.y, self.z, self.gx0, self.gx1, self.gy0, self.gy1, self.gz0, self.gz1)

        if self.use_density:
            set_terrain_density(self.x, self.y, self.z, self.density_threshold, self.base_tile)
        elif self.use_heightmap:
            set_terrain_height(self.x, self.y, self.z, self.baseline, self.maxheight, self.base_tile)

        elif self.dim == 1:
            print "Dimension 1 terrain not implemented."
        elif self.dim == 2:
            print "setting terrain height"
            set_terrain_height(self.x, self.y, self.z, self.baseline, self.maxheight, self.base_tile)
        elif self.dim == 3:
            set_terrain_density(self.x, self.y, self.z, self.density_threshold, self.base_tile)

        # features
        if self.add_grass:
            _grass(self.x, self.y, self.base)

        print 'map gen took %0.2f seconds' % (time.time() - _n)

        self.seed_index += 1
        self.seed_index %= len(self.seeds)
        set_seed(self.get_current_seed())

        return self


        
conf = Config()

def reset():
    clear_noisemap()

def apply_gradient1(x, x0, x1):
    apply_grad1(x, x0,x1)

def apply_gradient2(x,y, x0, x1, y0, y1):
    apply_grad2(x,y, x0, x1, y0, y1)

def apply_gradient3(x,y,z, x0, x1, y0, y1, z0, z1):
    apply_grad3(x,y,z,  x0, x1, y0, y1, z0, z1)

gradients = {
    1: apply_gradient1,
    2: apply_gradient2,
    3: apply_gradient3,
}

def apply_interpolate1(x, ix,rx,b, noise_type='p', rmf=False):
    if noise_type.startswith('p'):
        if rmf:
            apply_interp1_rmf_perlin(x, ix,rx,b)
        else:
            apply_interp1_perlin(x, ix,rx,b)
    else:
        print "apply interpolate 1 :: unrecognized noise type - %s" % (noise_type,)
    
def apply_interpolate2(x,y, ix,iy,rx,ry,b, noise_type='p', rmf=False):
    if noise_type.startswith('p'):
        if rmf:
            apply_interp2_rmf_perlin(x,y, ix, iy,rx,ry,b)
        else:
            apply_interp2_perlin(x,y, ix, iy,rx,ry,b)
    elif noise_type.startswith('s'):
        if rmf:
            apply_interp2_rmf_simplex(x,y, ix,iy)
        else:
            apply_interp2_simplex(x,y, ix,iy)
    else:
        print "apply interpolate 2 :: unrecognized noise type - %s" % (noise_type,)
    
def apply_interpolate3(x,y,z, ix,iy,iz,rx,ry,rz,b, noise_type='p', rmf=False):
    if noise_type.startswith('p'):
        if rmf:
            apply_interp3_rmf_perlin(x,y,z, ix,iy,iz,rx,ry,rz,b)
        else:
            apply_interp3_perlin(x,y,z, ix,iy,iz,rx,ry,rz,b)
    elif noise_type.startswith('s'):
        if rmf:
            apply_interp3_rmf_simplex(x,y,z, ix,iy,iz)
        else:
            apply_interp3_simplex(x,y,z, ix,iy,iz)
    else:
        print "apply interpolate 3 :: unrecognized noise type - %s" % (noise_type,)
        
interpolates = {
    1: apply_interpolate1,
    2: apply_interpolate2,
    3: apply_interpolate3,
}

def generate():
    conf.start()

def invert(x=xmax, y=ymax, z=zmax, tile=2):
    invert_map(x,y,z,tile)

def caves(x, y, z):
    _caves(x,y,z, 2)
