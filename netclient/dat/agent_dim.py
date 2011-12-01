
vosize = .0625

l_len = 20      # leg length

lu1 = [             # dimensions
[6,8,6],
[12,12,12],
[4,12,4],
[4,12,4],
[4,4,l_len],    # leg1
[4,4,l_len]     # leg2
]

lu2 = [             # anchor points (length, anchor [x,y,z])
[0, 0,0,2.5],
[0, 0,0,1.875],
[0, 0,-0.50,1.75],
[0, 3,0.50,1.75],
[0, 0,-0.1875,0.625],
[0, 0,0.1875,0.625],
]

lu3 = [             # orientations
[1,0,0, 0,0,1],     # head
[1,0,0, 0,0,1],     # torso
[1,0,0, 0,0,1],     # arm
[1,0,0, 0,0,1],     # block infront of agent (misplaced arm)
[1,0,0, 0,1,0],     # leg
[1,0,0, 0,-1,0],    # leg
]

from math import sin, cos, pi
i = 0.0
d = 1
def skel_tick():
    return
    global i,d, lu3
    rate = 1.0
    #if d == 1:
        #i+= rate
    #if d == -1:
        #i-= rate

    i += d * rate
    
    if i > 32:
        d = -1
    if i < -32:
        d = 1
    #pi = 3.1415
    i += 0.1
    lu3[4] = [sin(i/128*pi),0,cos(i/128*pi), 0,1,0]
    lu3[5] = [cos(i/128*pi),0,sin(i/128*pi), 0,-1,0]

def save():
    import json
    with open("agent_alignment.json", "w") as f:
        obj = {
            'lu1': lu1,
            'lu2': lu2,
            'lu3': lu3,
        }
        json.dump(obj, f)

def load():
    global lu1, lu2, lu3
    import json
    with open("agent_alignment.json") as f:
        obj = json.load(f)
        lu1 = obj['lu1']
        lu2 = obj['lu2']
        lu3 = obj['lu3']

import os.path
if os.path.exists("agent_alignment.json"):
    load()
