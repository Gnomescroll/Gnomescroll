flag =\
{
    'dim': (8,8,8),
    'vosize': 0.2,
    'voxels': [\
        # RED
        [1,1,1, 255, 0, 0, 255],    # row 1
        [1,2,1, 255, 0, 0, 255],
        [1,3,1, 255, 0, 0, 255],
        [1,4,1, 255, 0, 0, 255],
        [1,5,1, 255, 0, 0, 255],
        
        [1,1,2, 255, 0, 0, 255],    # row 2
        [1,3,2, 255, 0, 0, 255],
        [1,5,2, 255, 0, 0, 255],

        [1,1,3, 255, 0, 0, 255],    # row 3
        [1,2,3, 255, 0, 0, 255],
        [1,3,3, 255, 0, 0, 255],
        [1,4,3, 255, 0, 0, 255],
        [1,5,3, 255, 0, 0, 255],
        
        [1,2,2, 0, 255, 0, 255],    # row 2
        [1,4,2, 0, 255, 0, 255],
    ],
}

black_flag =\
{
    'dim': (8,8,8),
    'vosize': 0.2,
    'voxels': [\
        # BLACK
        [1,1,1, 0, 0, 0, 255],    # row 1
        [1,2,1, 0, 0, 0, 255],
        [1,3,1, 0, 0, 0, 255],
        [1,4,1, 0, 0, 0, 255],
        [1,5,1, 0, 0, 0, 255],
        
        [1,1,2, 0, 0, 0, 255],    # row 2
        [1,3,2, 0, 0, 0, 255],
        [1,5,2, 0, 0, 0, 255],

        [1,1,3, 0, 0, 0, 255],    # row 3
        [1,2,3, 0, 0, 0, 255],
        [1,3,3, 0, 0, 0, 255],
        [1,4,3, 0, 0, 0, 255],
        [1,5,3, 0, 0, 0, 255],
        #GREEN
        [1,2,2, 0, 255, 0, 255],    # row 2
        [1,4,2, 0, 255, 0, 255],
    ],
}


import json

def _write_vox(fn, vox):
    with open('../media/vox/' + fn, 'w') as f:
        json.dump(vox, f)

def write_vox(fn, voxels, dim=(8,8,8), vosize=0.2):
    n = {
        'dim'   :   dim,
        'vosize':   vosize,
        'voxels':   voxels,
    }
    _write_vox(fn, n)

#_write_vox('flag.vox', flag)
#_write_vox('black_flag.vox', black_flag)

voxels = []
dim = (8,8,8)
x,y,z = dim

for i in range(x):
    for j in range(y):
        for k in range(z):
            loc = [i,j,k]
            col = [255 * (1.* i/x), 255 * (1.* j/y), 255 * (1.* k/z), 255]
            col = map(lambda x: int(x), col)
            loc.extend(col)
            voxels.append(loc)

#write_vox('auto.vox', voxels)

base = []
dim = (10,10,10)
_x, _y, _z = dim
x,y,z = [range(u) for u in dim]
corner_ct = 0
panel_edge = 0
for i in x:
    for j in y:
        for k in z:
            loc = [i,j,k]
            if i == 0 or i == _x-1:   # edge
                col = [0,0,0]
                alpha = 0
                if j == 0 or j == _y-1 or k == 0 or k == _z-1:
                    panel_edge += 1
                    alpha = 255
            else:
                alpha = 255
                if j == 0 or j == _y-1 or k == 0 or k == _z-1:
                    col = [0,0,0]
                    alpha = 0
                    if (j,k) in ((0,_y-1), (_y-1,0), (0,_z-1), (_z-1,0)):   # corner
                        corner_ct += 1
                        alpha = 255
                else:   # inner cube
                    col = [255 * (1.* i/_x), 255 * (1.* j/_y), 255 * (1.* k/_z)]
            col.append(alpha)

            col = map(int, col)
            loc.extend(col)
            base.append(loc)

print 'CORNERS %d' % (corner_ct,)
print 'PANEL EDGE %d' % (panel_edge,)
            

write_vox('base.vox', base, dim)


voxels = []
dim = (8,8,1)
x,y,z = dim
for i in range(x):
    for j in range(y):
        loc = [i,j,0]
        col = [255,100,100, 255]
        loc.extend(col)
        voxels.append(loc)

#write_vox('flag.vox', voxels, dim)
