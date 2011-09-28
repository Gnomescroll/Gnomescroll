cdef extern from "./map_gen/noise.c":
    float perlin1(float x, int octaves, float persistence, float frequency, float amplitude, int repeat, int base)
    float perlin2(float x, float y, int octaves, float persistence, float frequency, float amplitude, int repeatx, int repeaty, int base)
    float perlin3(float x, float y, float z, int octaves, float persistence, float frequency, float amplitude, int repeatx, int repeaty, int repeatz, int base)

    float simplex2(float x, float y, int octaves, float persistence, float frequency, float amplitude)
    float simplex3(float x, float y, float z, int octaves, float persistence, float frequency, float amplitude)
    
    float rmf_perlin1(float rmf_x, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude, int repeat, int base)
    float rmf_perlin2(float rmf_x, float rmf_y, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude, int repeatx, int repeaty, int base)
    float rmf_perlin3(float rmf_x, float rmf_y, float rmf_z, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude, int repeatx, int repeaty, int repeatz, int base)
    float rmf_simplex2(float rmf_x, float rmf_y, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude)
    float rmf_simplex3(float rmf_x, float rmf_y, float rmf_z, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude)

    void seed_noise(int seed)

    void interp(int x, int y, int z, int x_interval, int y_interval, int z_interval)

    void apply_grad3d(int x, int y, int z, float x_pos, float y_pos, float z_pos, float x_neg, float y_neg, float z_neg)



def set_seed(seed):
    # convert seed to int
    seed_noise(seed)

class Perlin:
    def __init__(self, octaves=1, persistence=0.5, frequency=1.0, amplitude=1.0, repeatx=1024, repeaty=1024, repeatz=1024, base=0):
        self.octaves = octaves
        self.persistence = persistence
        self.frequency = frequency
        self.amplitude = amplitude

        self.repeatx = repeatx
        self.repeaty = repeaty
        self.repeatz = repeatz
        self.base = base

    def noise1(self, x):
        return perlin1(x, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeat, self.base)

    def noise2(self, x,y):
        cdef float n
        n = perlin2(x, y, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeatx, self.repeaty, self.base)
        return n

    def noise3(self, x,y,z):
        return perlin3(x, y, z, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeatx, self.repeaty, self.repeatz, self.base)


class Simplex:

    def __init__(self, octaves=1, persistence=0.5, frequency=1.0, amplitude=1.0):
        self.octaves = octaves
        self.persistence = persistence
        self.frequency = frequency
        self.amplitude = amplitude

    def noise2(self, x,y):
        return simplex2(x, y, self.octaves, self.persistence, self.frequency, self.amplitude)
        
    def noise3(self, x,y,z):
        return simplex3(x, y, z, self.octaves, self.persistence, self.frequency, self.amplitude)

        
class RMF:
    def __init__(self, octaves=1, persistence=0.5, frequency=1.0, amplitude=1.0, repeatx=1024, repeaty=1024, repeatz=1024, base=0):
        self.octaves = octaves
        self.persistence = persistence
        self.frequency = frequency
        self.amplitude = amplitude

        self.repeatx = repeatx
        self.repeaty = repeaty
        self.repeatz = repeatz
        self.base = base

    def pnoise1(self, x):
        return rmf_perlin1(x, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeat, self.base)
        
    def pnoise2(self, x,y):
        return rmf_perlin2(x, y, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeatx, self.repeaty, self.base)

    def pnoise3(self, x,y,z):
        return rmf_perlin3(x, y, z, self.octaves, self.persistence, self.frequency, self.amplitude, self.repeatx, self.repeaty, self.repeatz, self.base)

    def snoise2(self, x,y):
        return rmf_simplex2(x, y, self.octaves, self.persistence, self.frequency, self.amplitude)
        
    def snoise3(self, x,y,z):
        return rmf_simplex3(x, y, z, self.octaves, self.persistence, self.frequency, self.amplitude)


def interpolate(x,y,z, ix,iy,iz):
    interp(x,y,z, ix,iy,iz)

def gradient(x,y,z):
    print "xrssassdsssssda"
    apply_grad3d(x,y,z, 0.0, 0.0, 0.5, 0.0, 0.0, -0.5)

