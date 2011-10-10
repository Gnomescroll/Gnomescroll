cdef extern from "./map_gen/perlin.h":
    float perlin1(float x, int repeat, int base)
    float perlin2(float x, float y, int repeatx, int repeaty, int base)
    float perlin3(float x, float y, float z, int repeatx, int repeaty, int repeatz, int base)
    
    void perlin1_fill(int x, int repeat, int base)
    void perlin2_fill(int x, int y, int repeatx, int repeaty, int base)
    void perlin3_fill(int x, int y, int z, int repeatx, int repeaty, int repeatz, int base)

cdef extern from "./map_gen/simplex.h":
    float simplex2(float x, float y)
    float simplex3(float x, float y, float z)

    void simplex2_fill(int x, int y)
    void simplex3_fill(int x, int y, int z)
    
cdef extern from "./map_gen/ridged_mf.h":
    float rmf_perlin1(float rmf_x, int repeat, int base)
    float rmf_perlin2(float rmf_x, float rmf_y, int repeatx, int repeaty, int base)
    float rmf_perlin3(float rmf_x, float rmf_y, float rmf_z, int repeatx, int repeaty, int repeatz, int base)
    float rmf_simplex2(float rmf_x, float rmf_y)
    float rmf_simplex3(float rmf_x, float rmf_y, float rmf_z)

    void rmf_perlin1_fill(int rmf_x, int repeat, int base)
    void rmf_perlin2_fill(int rmf_x, int rmf_y, int repeatx, int repeaty, int base)
    void rmf_perlin3_fill(int rmf_x, int rmf_y, int rmf_z, int repeatx, int repeaty, int repeatz, int base)
    void rmf_simplex2_fill(int rmf_x, int rmf_y)
    void rmf_simplex3_fill(int rmf_x, int rmf_y, int rmf_z)

cdef extern from "./map_gen/noise.h":
    void seed_noise(int seed)
    void set_noise_parameters(int octaves, float persistence, float amplitude, float lacunarity, float frequency)

def set_seed(seed):
    seed_noise(seed)

class Perlin:
    def __init__(self, octaves=1, persistence=0.5, amplitude=1.0, lacunarity=2.0, frequency=1.0,
                       repeatx=1024, repeaty=1024, repeatz=1024, base=0):

        self.octaves = octaves
        self.persistence = persistence
        self.frequency = frequency
        self.amplitude = amplitude
        self.lacunarity = lacunarity

        self.repeatx = repeatx
        self.repeaty = repeaty
        self.repeatz = repeatz
        self.base = base

        self.filled = False

        set_noise_parameters(self.octaves, self.persistence, self.amplitude, self.lacunarity, self.frequency)

    def fill(self):
        self.filled = True
        return self

    def noise1(self, x):
        if self.filled:
            self.filled = False
            perlin1_fill(x, self.repeat, self.base)
        else:
            return perlin1(x, self.repeat, self.base)

    def noise2(self, x,y):
        if self.filled:
            print 'noise 2 filled'
            self.filled = False
            perlin2_fill(x, y, self.repeatx, self.repeaty, self.base)
        else:
            return perlin2(x, y, self.repeatx, self.repeaty, self.base)

    def noise3(self, x,y,z):
        if self.filled:
            self.filled = False
            perlin3_fill(x, y, z, self.repeatx, self.repeaty, self.repeatz, self.base)
        else:
            return perlin3(x, y, z, self.repeatx, self.repeaty, self.repeatz, self.base)


class Simplex:

    def __init__(self, octaves=1, persistence=0.5, amplitude=1.0, lacunarity=2.0, frequency=1.0):
        self.octaves = octaves
        self.persistence = persistence
        self.frequency = frequency
        self.amplitude = amplitude
        self.lacunarity = lacunarity

        self.filled = False

        set_noise_parameters(self.octaves, self.persistence, self.amplitude, self.lacunarity, self.frequency)

    def fill(self):
        self.filled = True
        return self

    def noise2(self, x,y):
        if self.filled:
            self.filled = False
            simplex2_fill(x, y)
        else:
            return simplex2(x, y)
        
    def noise3(self, x,y,z):
        if self.filled:
            self.filled = False
            simplex3_fill(x, y, z)
        else:
            return simplex3(x, y, z)

        
class RMF:
    def __init__(self, octaves=1, persistence=0.5, amplitude=1.0, lacunarity=2.0, frequency=1.0,
                       repeatx=1024, repeaty=1024, repeatz=1024, base=0):

        self.octaves = octaves
        self.persistence = persistence
        self.frequency = frequency
        self.amplitude = amplitude
        self.lacunarity = lacunarity

        self.repeatx = repeatx
        self.repeaty = repeaty
        self.repeatz = repeatz
        self.base = base

        self.filled = False

        set_noise_parameters(self.octaves, self.persistence, self.amplitude, self.lacunarity, self.frequency)

    def fill(self):
        self.filled = True
        return self

    def pnoise1(self, x):
        if self.filled:
            self.filled = False
            rmf_perlin1_fill(x, self.repeat, self.base)
        else:
            return rmf_perlin1(x, self.repeat, self.base)
        
    def pnoise2(self, x,y):
        print 'cython rmf pnoise2'
        if self.filled:
            self.filled = False
            print 'cython rmf perlin 2 fill'
            rmf_perlin2_fill(x, y, self.repeatx, self.repeaty, self.base)
        else:
            return rmf_perlin2(x, y, self.repeatx, self.repeaty, self.base)

    def pnoise3(self, x,y,z):
        if self.filled:
            self.filled = False
            rmf_perlin3_fill(x, y, z, self.repeatx, self.repeaty, self.repeatz, self.base)
        else:
            return rmf_perlin3(x, y, z, self.repeatx, self.repeaty, self.repeatz, self.base)

    def snoise2(self, x,y):
        if self.filled:
            self.filled = False
            rmf_simplex2_fill(x, y)
        else:
            return rmf_simplex2(x, y)
        
    def snoise3(self, x,y,z):
        if self.filled:
            self.filled = False
            rmf_simplex3_fill(x, y, z)
        else:
            return rmf_simplex3(x, y, z)
