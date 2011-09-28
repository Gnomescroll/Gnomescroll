cdef extern from "./map_gen/noise.c"
    void interp(int x, int y, int z, int x_interval, int y_interval, int z_interval)

    void apply_grad3d(int x, int y, int z, float x_pos, float y_pos, float z_pos, float x_neg, float y_neg, float z_neg)


class Config:

    def size(self, x,y,z):
        self.x = x
        self.y = y
        self.z = z

    def interpolate(self, x,y,z):
        self.ix = x
        self.iy = y
        self.iz = z

    def gradient(x0=0.0, x1=0.0, y0=0.0, y1=0.0, z0=0.0, z1=0.0)

conf = Config()


def interpolate(x,y,z, ix,iy,iz):
    interp(x,y,z, ix,iy,iz)

def gradient(x,y,z):
    print "xrssassdsssssda"
    apply_grad3d(x,y,z, 0.0, 0.0, 0.5, 0.0, 0.0, -0.5)



def generate():
    pass
