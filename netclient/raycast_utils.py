### Ray casting utilities

from math import sin, cos, sqrt, pi
from c_lib.terrain_map import collisionDetection

def farthest_empty_block(pos, vector, max_distance= 4., z_low=4, z_high=3 ):

    sampling_density = 100.00

    x,y,z = pos
    dx,dy,dz = vector
    le = sqrt(dx**2+dy**2+dz**2)
    dx /= le
    dy /= le
    dz /= le

    n = 0.
    inc = 1. / sampling_density
    xy_inc = sqrt(dx**2 + dy**2)

    if dz >= 0:
        z_max = z_high
        z_inc = dz
    else:
        z_max = z_low
        z_inc = -1*dz

    #md2 = max_distance**2
    while True:
        n += inc
        if n*xy_inc > max_distance or n*z_inc > z_max:
            return None

        x_ = int(x+dx*n)
        y_ = int(y+dy*n)
        z_ = int(z+dz*n)

        x__ = int(x+ dx*(n+inc))
        y__ = int(y+ dy*(n+inc))
        z__ = int(z+ dz*(n+inc))

        if x_ != x__ or y_ != y__ or z_ != z__:
            if collisionDetection(x__, y__, z__):
                #z = int(z)
                if z_ >= z-z_low and z_ <= z+z_high:
                    return (x_, y_, z_)
                else:
                    #print "out of range:" + str((x_, y_, z_))
                    return None

def nearest_block(pos, vector, max_distance= 4., z_low=4, z_high=3 ):
    sampling_density = 100.00

    x,y,z = pos
    dx,dy,dz = vector
    le = sqrt(dx**2+dy**2+dz**2)
    dx /= le
    dy /= le
    dz /= le

    n = 0.
    inc = 1. / sampling_density
    xy_inc = sqrt(dx**2 + dy**2)

    if dz > 0:
        z_max = z_high
        z_inc = dz
    else:
        z_max = z_low
        z_inc = -1*dz
    #md2 = max_distance**2
    while True:
        n += inc
        if n*xy_inc > max_distance or n*z_inc > z_max:
            return None

        x_ = int(x+dx*n)
        y_ = int(y+dy*n)
        z_ = int(z+dz*n)

        x__ = int(x+ dx*(n+inc))
        y__ = int(y+ dy*(n+inc))
        z__ = int(z+ dz*(n+inc))

        if x_ != x__ or y_ != y__ or z_ != z__:
            if collisionDetection(x__, y__, z__):
                return (x__, y__, z__)

from game_state import GameStateGlobal
