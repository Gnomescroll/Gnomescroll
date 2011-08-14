'''
Vector math
'''

from math import pi, sqrt, cos, sin

# adds n*vector to point
# use for normalized vectors
def move_point(pt, v, n):
    v = [i*n for i in v]
    end = [a[0]+a[1] for a in zip(pt, v)]
    return end

def normalize(v):
    scale = sqrt(sum([i**2 for i in v]))
    v = [i/scale for i in v]
    return v

def angle2vector(x_angle, y_angle):
    x1 = cos( x_angle * pi) * cos( y_angle * pi);
    y1 = sin( x_angle * pi) * cos( y_angle * pi);
    z1 = sin( y_angle);
    return [x1, y1, z1]

def distance(pt1, pt2=None):
    if pt2 is None:
        pt2 = [0,0,0]
    dist = sqrt(sum([(a[0] - a[1])**2 for a in zip(pt1, pt2)]))
    return dist

def vector_between_points(pt1, pt2):
    return [a-b for a,b in zip(pt2, pt1)]
