
#include <stdio.h>
#include <stdlib.h>

#include "types.h"

#define vm_map_dim 64 //number of map chunks in x/y
#define vm_chunk_size 8
#define vm_column_max 16

struct vm_chunk {
    unsigned short voxel[512];
    int x_off, y_off, z_off; //lower corner of chunk
    unsigned int local_version;
    unsigned int server_version;
};

struct vm_column {
    int x_off, y_off;
    struct vm_chunk* chunk[vm_column_max];
    unsigned int local_version;
    unsigned int server_version;
    //vm_column_history history;
};

struct vm_map {
    struct vm_column column[vm_map_dim*vm_map_dim];
};

//functions
extern struct vm_map map;

int init_t_map();
int _set(int x, int y, int z, int value);
int _get(int x, int y, int z);
struct vm_map* _get_map();
struct vm_chunk* _get_chunk(int xoff, int yoff, int zoff);
