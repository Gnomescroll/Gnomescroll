cdef extern from "interface.hpp":
    void init_p()
    double p3(double x, double y, double z)

def _init():
    init_p()

def _p3(double x, double y, double z):
    return p3(x,y,z)
