
#import "t_map.h"
#include <stdio.h>

//globals

struct vm_map map;

int init_t_map() {
    return 0;
}

struct vm_map* _get_map() { return &map; }

// terrain map tile set/get
int _set(int x, int y, int z, int value){
    int xoff, yoff, zoff, xrel, yrel, zrel;
    struct vm_column* column;
    struct vm_chunk* chunk;
    xoff = x >> 3; yoff = y >> 3; zoff = z >> 3;
    if(xoff < 0 || xoff >= vm_map_dim || yoff < 0 || yoff >= vm_map_dim || zoff < 0 || vm_column_max >= vm_column_max) {
        printf("t_map set: invalid Assignment to map: (%i, %i, %i, %i)\n", x,y,z,value);
        return 0;
    }
    xrel = x - (xoff << 3); yrel; = y - (yoff << 3); zrel = z - (zoff) << 3);
    column = &map.chunk[vm_map_dim*yoff + xoff];
    chunk = column[zoff];
    if(chunk == NULL) {
        column[zoff] = (struct vm_chunk*)malloc(sizeof(struct vm_chunk));
        chunk = column[zoff];
        chunk->x_off = xoff;chunk->y_off=yoff;chunk->z_off=zoff;
    }
    chunk->voxel[vm_chunk_size*vm_chunk_size*zrel+ vm_chunk_size*yrel + xrel] = value;
    column->local_version++;
    chunk->local_version++;
    return 0;
}

int _get(int x, int y, int z) {
    int xoff, yoff, zoff;
    struct vm_column* column;
    struct vm_chunk* chunk;
    xoff = x >> 3; yoff = y >> 3; zoff = z >> 3;
    if(xoff < 0 || xoff >= vm_map_dim || yoff < 0 || yoff >= vm_map_dim || zoff < 0 || vm_column_max >= vm_column_max) return 0;
    xrel = x - (xoff << 3); yrel; = y - (yoff << 3); zrel = z - (zoff) << 3);
    column = &map.chunk[vm_map_dim*yoff + xoff];
    chunk = column.chunk[zoff];
    if(chunk == NULL) return 0;
    return chunk->voxel[vm_chunk_size*vm_chunk_size*zrel+ vm_chunk_size*yrel + xrel];
}

struct vm_chunk* _get_chunk(int xoff, int yoff, int zoff){
    struct vm_column* column;
    struct vm_chunk* chunk;
    if(xoff < 0 || xoff >= vm_map_dim || yoff < 0 || yoff >= vm_map_dim || zoff < 0 || vm_column_max >= vm_column_max) {
        printf("t_map _get_chunk: parameters out of range\n"); return NULL;
    }
    column = &map.chunk[vm_map_dim*yoff + xoff];
    chunk = column.chunk[zoff];
    return chunk;
}
