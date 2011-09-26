cdef extern from "interface.hpp":
    void init_p()
    double p3(double x, double y, double z)
    double Vo(double x, double y, double z, int a, int b)
    double p4(int x, int y, double x1, double y1, double x2, double y2, double height, double width)

def _init():
    init_p()

def _p3(double x, double y, double z):
    return p3(x,y,z)

def _Vo(double x, double y, double z, int a, int b):
    return Vo(x,y,z, a,b)


def _p4T(int x, int y, double x1, double y1, double x2, double y2, double height, double width):
    return p4(x, y, x1,y1,x2,y2, height, width)
