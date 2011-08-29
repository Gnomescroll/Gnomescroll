
vosize = .0625

l_len = 12

lu1 = [
[8,8,8],
[12,12,16],
[4,12,4],
[4,12,4],
[4,4,l_len],
[4,4,l_len]
]

lu2 = [
[0, 0,0,3.0],
[0, 0,0,2],
[0, -3.0,0,0],
[0, 3,0,0],
[vosize*l_len/2, 0,1,1.30],
[vosize*l_len/2, 0,-1,1.30],
]

lu3 = [
[1,0,0],
[1,0,0],
[1,0,0],
[1,0,0],
[1,0,0],
[1,0,0],
]

from math import sin, cos
i = 0.0
d = 1
def skel_tick():
    global i,d, lu3
    rate = 1.0
    if d == 1:
        i+= rate
    if d == -1:
        i-= rate
    if i > 32:
        d = -1
    if i < -32:
        d = 1
    pi = 3.1415
    i += 0.1
    lu3[4] = [sin(i/128*pi),0,cos(i/128*pi)]
    lu3[5] = [cos(i/128*pi),0,sin(i/128*pi)]
