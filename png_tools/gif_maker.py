#!/usr/bin/python

def gif_maker():
    z_step = 0.0013
    freq = 9
    seed = 9090
    distance_enabled = True
    w = 256
    h = 256
    gradient = 6
    turb_enable = False
    turb_f = 1
    turb_pow = 1
    init_c_lib.VN.configure(z_step, freq, seed, distance_enabled, turb_enable, turb_f, turb_pow)
    #init_c_lib.VN.configure(z_step, freq, seed, distance_enabled, turb_f, turb_pow)
    init_c_lib.VN.init(width=w, height=h, gradient=gradient)
    print 'preparing to generate frames'
    secs = 1./30.
    #secs = 1.
    init_c_lib.VN.frames(secs)
    print 'frames generated'
    sys.exit()

"""
Voronoi texture surface
"""
#from libcpp cimport bool
#cdef extern from "SDL/v.hpp" namespace "vn":
#    void draw_vn()
#    void generate_frames(float seconds)
#    void set_params(double dz, double frequency, int seed, bool distance, bool turbulence_enabled, double turbulence_frequency, double turbulence_power)
##    void set_params(double dz, double frequency, int seed, bool distance, double turbulence_frequency, double turbulence_power)
#    void init_vn(int width, int height, int gradient)

#class VN(object):
#    @classmethod
#    def draw(cls):
#        draw_vn()
#    @classmethod
#    def frames(cls, float seconds):
#        generate_frames(seconds)
#    @classmethod
#    def configure(cls, double dz, double frequency, int seed, bool distance, bool turbulence_enabled, double turbulence_frequency, double turbulence_power):
##    def configure(cls, double dz, double frequency, int seed, bool distance, double turbulence_frequency, double turbulence_power):
#        set_params(dz, frequency, seed, distance, turbulence_enabled, turbulence_frequency, turbulence_power)
##        set_params(dz, frequency, seed, distance, turbulence_frequency, turbulence_power)
#    @classmethod
#    def init(cls, int width, int height, int gradient):
#        init_vn(width, height, gradient)

