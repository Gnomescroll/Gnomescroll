cdef extern from "interface.hpp":
    void init_p()
    double p3(double x, double y, double z)
    double Vo(double x, double y, double z, int a, int b)

def _init():
    init_p()

def _p3(double x, double y, double z):
    return p3(x,y,z)

def _Vo(double x, double y, double z, int a, int b):
    return Vo(x,y,z, a,b)
