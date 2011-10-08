cdef extern from "./map_gen/perlin.h":
    float perlin1(float x, int octaves, float persistence, float frequency, float amplitude, int repeat, int base)
    float perlin2(float x, float y, int octaves, float persistence, float frequency, float amplitude, int repeatx, int repeaty, int base)
    float perlin3(float x, float y, float z, int octaves, float persistence, float frequency, float amplitude, int repeatx, int repeaty, int repeatz, int base)
    
    void perlin1_fill(int x, int octaves, float persistence, float frequency, float amplitude, int repeat, int base)
    void perlin2_fill(int x, int y, int octaves, float persistence, float frequency, float amplitude, int repeatx, int repeaty, int base)
    void perlin3_fill(int x, int y, int z, int octaves, float persistence, float frequency, float amplitude, int repeatx, int repeaty, int repeatz, int base)

cdef extern from "./map_gen/simplex.h":
    float simplex2(float x, float y, int octaves, float persistence, float frequency, float amplitude)
    float simplex3(float x, float y, float z, int octaves, float persistence, float frequency, float amplitude)

    void simplex2_fill(int x, int y, int octaves, float persistence, float frequency, float amplitude)
    void simplex3_fill(int x, int y, int z, int octaves, float persistence, float frequency, float amplitude)
    
cdef extern from "./map_gen/ridged_mf.h":
    float rmf_perlin1(float rmf_x, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude, int repeat, int base)
    float rmf_perlin2(float rmf_x, float rmf_y, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude, int repeatx, int repeaty, int base)
    float rmf_perlin3(float rmf_x, float rmf_y, float rmf_z, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude, int repeatx, int repeaty, int repeatz, int base)
    float rmf_simplex2(float rmf_x, float rmf_y, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude)
    float rmf_simplex3(float rmf_x, float rmf_y, float rmf_z, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude)

    void rmf_perlin1_fill(int rmf_x, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude, int repeat, int base)
    void rmf_perlin2_fill(int rmf_x, int rmf_y, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude, int repeatx, int repeaty, int base)
    void rmf_perlin3_fill(int rmf_x, int rmf_y, int rmf_z, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude, int repeatx, int repeaty, int repeatz, int base)
    void rmf_simplex2_fill(int rmf_x, int rmf_y, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude)
    void rmf_simplex3_fill(int rmf_x, int rmf_y, int rmf_z, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude)

cdef extern from "./map_gen/noise.h":
    void seed_noise(int seed)

def set_seed(seed):
    seed_noise(seed)

class Perlin:
    def __init__(self, octaves=1, persistence=0.5, frequency=1.0, amplitude=1.0,
                       repeatx=1024, repeaty=1024, repeatz=1024, base=0):

        self.octaves = octaves
        self.persistence = persistence
        self.frequency = frequency
        self.amplitude = amplitude

        self.repeatx = repeatx
        self.repeaty = repeaty
        self.repeatz = repeatz
        self.base = base

        self.filled = False

    def fill(self):
        self.filled = True
        return self

    def noise1(self, x):
        if self.filled:
            self.filled = False
            perlin1_fill(x, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeat, self.base)
        else:
            return perlin1(x, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeat, self.base)

    def noise2(self, x,y):
        if self.filled:
            print 'noise 2 filled    '
            self.filled = False
            perlin2_fill(x, y, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeatx, self.repeaty, self.base)
        else:
            return perlin2(x, y, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeatx, self.repeaty, self.base)

    def noise3(self, x,y,z):
        if self.filled:
            self.filled = False
            perlin3_fill(x, y, z, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeatx, self.repeaty, self.repeatz, self.base)
        else:
            return perlin3(x, y, z, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeatx, self.repeaty, self.repeatz, self.base)


class Simplex:

    def __init__(self, octaves=1, persistence=0.5, frequency=1.0, amplitude=1.0):
        self.octaves = octaves
        self.persistence = persistence
        self.frequency = frequency
        self.amplitude = amplitude

        self.filled = False

    def fill(self):
        self.filled = True
        return self

    def noise2(self, x,y):
        if self.filled:
            self.filled = False
            simplex2_fill(x, y, self.octaves, self.persistence, self.frequency, self.amplitude)
        else:
            return simplex2(x, y, self.octaves, self.persistence, self.frequency, self.amplitude)
        
    def noise3(self, x,y,z):
        if self.filled:
            self.filled = False
            simplex3_fill(x, y, z, self.octaves, self.persistence, self.frequency, self.amplitude)
        else:
            return simplex3(x, y, z, self.octaves, self.persistence, self.frequency, self.amplitude)

        
class RMF:
    def __init__(self, octaves=1, persistence=0.5, frequency=1.0, amplitude=1.0,
                       repeatx=1024, repeaty=1024, repeatz=1024, base=0):

        self.octaves = octaves
        self.persistence = persistence
        self.frequency = frequency
        self.amplitude = amplitude

        self.repeatx = repeatx
        self.repeaty = repeaty
        self.repeatz = repeatz
        self.base = base

        print 'cython rmf init'
        self.filled = False

    def fill(self):
        self.filled = True
        return self

    def pnoise1(self, x):
        if self.filled:
            self.filled = False
            rmf_perlin1_fill(x, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeat, self.base)
        else:
            return rmf_perlin1(x, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeat, self.base)
        
    def pnoise2(self, x,y):
        print 'cython rmf pnoise2'
        if self.filled:
            self.filled = False
            print 'cython rmf perlin 2 fill'
            rmf_perlin2_fill(x, y, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeatx, self.repeaty, self.base)
        else:
            return rmf_perlin2(x, y, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeatx, self.repeaty, self.base)

    def pnoise3(self, x,y,z):
        if self.filled:
            self.filled = False
            rmf_perlin3_fill(x, y, z, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeatx, self.repeaty, self.repeatz, self.base)
        else:
            return rmf_perlin3(x, y, z, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeatx, self.repeaty, self.repeatz, self.base)

    def snoise2(self, x,y):
        if self.filled:
            self.filled = False
            rmf_simplex2_fill(x, y, self.octaves, self.persistence, self.frequency, self.amplitude)
        else:
            return rmf_simplex2(x, y, self.octaves, self.persistence, self.frequency, self.amplitude)
        
    def snoise3(self, x,y,z):
        if self.filled:
            self.filled = False
            rmf_simplex3_fill(x, y, z, self.octaves, self.persistence, self.frequency, self.amplitude)
        else:
            return rmf_simplex3(x, y, z, self.octaves, self.persistence, self.frequency, self.amplitude)
