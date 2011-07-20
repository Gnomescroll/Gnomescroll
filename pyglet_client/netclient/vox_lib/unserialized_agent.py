agent =\
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


import json

def write_vox(fn, vox):
    with open('../media/vox/' + fn, 'w') as f:
        json.dump(vox, f)

write_vox('agent.vox', agent)

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

n = \
{
    'dim':  dim,
    'vosize': 0.2,
    'voxels': voxels,
}
write_vox('auto.vox', n)
