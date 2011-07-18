'''
Vector math
'''

from math import pi, sqrt

# adds n*vector to point
# use for normalized vectors
def move_point(pt, v, n):
    v = [i*n for n in v]
    end = [a[0]+a[1] for a in zip(pt, v)]
    return end

def normalize(v):
    scale = sqrt(sum([i**2 for i in v]))
    x1 /= s; y1 /=s; z1 /=s;
    v = [i/s for i in v]
    return v

def angle2vector(x_angle, y_angle):
    x1 = cos( x_angle * pi) * cos( y_angle * pi);
    y1 = sin( x_angle * pi) * cos( y_angle * pi);
    z1 = sin( y_angle);
    return [x1, y1, z1]

def distance(self, pt1, pt2):
    dist = sqrt(sum([(a[0] - a[1])**2 for a in zip(pt1, pt2)]))
    return dist
