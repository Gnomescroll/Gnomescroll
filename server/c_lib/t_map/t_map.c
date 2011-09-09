#include "t_map.h"


struct vm_map map;

int _init_t_map() {

    int i;
    struct vm_column* c;
    for(i=0; i<vm_map_dim*vm_map_dim; i++) {
        c = &map.column[i];
        c->vbo.v_list = NULL;
        c->vbo.v_num = 0;
        c->vbo.VBO_id = 0;
        c->x_off = i % vm_map_dim;
        c->y_off = i/vm_map_dim;
        c->flag = 0; //null it
        //c->vbo_needs_update = 0;
        //set_flag(c, VBO_loaded, 0);
        //set_flag(c, VBO_needs_update, 0);
        //set_flag(c, VBO_has_blocks, 0);
    }
    return 0;
}

struct vm_map* _get_map() { return &map; }

struct vm_chunk* new_chunk(int xoff,int yoff,int zoff) {
    int i;
    struct vm_chunk* chunk;
    chunk = (struct vm_chunk*) malloc(sizeof(struct vm_chunk));
    chunk->x_off = xoff;chunk->y_off=yoff;chunk->z_off=zoff;
    chunk->local_version = 512;
    chunk->server_version = 512; //not used on server
    for(i=0; i<512;i++){
    chunk->voxel[i] = 0;
    }
    return chunk;
}

//if apply damage returns 1, then handle block destruction
int _apply_damage(int x, int y, int z, int value) {
    int xoff, yoff, zoff, xrel, yrel, zrel;
    int tile;
    struct vm_column* column;
    struct vm_chunk* chunk;
    xoff = x >> 3; yoff = y >> 3; zoff = z >> 3;
    xrel = x - (xoff << 3); yrel = y - (yoff << 3); zrel = z - (zoff << 3);
    column = &map.column[vm_map_dim*yoff + xoff];
    chunk = column->chunk[zoff];
    if(chunk == NULL) { printf("!!! _apply_damage: THIS CANNOT OCCUR< EVER\n"); return 0; }
    tile = chunk->voxel[vm_chunk_size*vm_chunk_size*zrel+ vm_chunk_size*yrel + xrel];
    unsigned char* damage;
    damage = &chunk->damage[vm_chunk_size*vm_chunk_size*zrel+ vm_chunk_size*yrel + xrel];
    *damage += value;
    if(*damage >= _maxDamage(tile) ) {
        return 1;
    } else {
        return 0;
    }
}

// terrain map tile set/get
int _set(int x, int y, int z, int value) {
    int xoff, yoff, zoff, xrel, yrel, zrel;
    struct vm_column* column;
    struct vm_chunk* chunk;
    xoff = x >> 3; yoff = y >> 3; zoff = z >> 3;
    if(xoff < 0 || xoff >= vm_map_dim || yoff < 0 || yoff >= vm_map_dim || zoff < 0 || zoff >= vm_column_max) {
        printf("t_map set: invalid Assignment to map: (%i, %i, %i, %i)\n", x,y,z,value);
        return 0;
    }
    xrel = x - (xoff << 3); yrel = y - (yoff << 3); zrel = z - (zoff << 3);
    //printf("xoff,yoff,zoff= %i, %i, %i \n", xoff,yoff,zoff);
    //printf("xrel,yrel,zrel= %i, %i, %i \n", xrel, yrel, zrel);
    column = &map.column[vm_map_dim*yoff + xoff];
    chunk = column->chunk[zoff];
    if(chunk == NULL) {
        //printf("creating new chunk \n");
        //printf("xoff,yoff,zoff= %i, %i, %i \n", xoff,yoff,zoff);
        chunk = new_chunk(xoff, yoff, zoff);
        column->chunk[zoff] = chunk;
    }
    chunk->voxel[vm_chunk_size*vm_chunk_size*zrel+ vm_chunk_size*yrel + xrel] = value;
    chunk->damage[vm_chunk_size*vm_chunk_size*zrel+ vm_chunk_size*yrel + xrel] = 0;
    column->local_version++;
    column->server_version++;
    //set_flag(column, VBO_has_blocks, 1);
    chunk->local_version++;
    //column->vbo_needs_update = 1;
    //chunk->vbo_needs_update =1;
    return 0;
}

int _get(int x, int y, int z) {
    int xoff, yoff, zoff, xrel, yrel, zrel;
    struct vm_column* column;
    struct vm_chunk* chunk;
    xoff = x >> 3; yoff = y >> 3; zoff = z >> 3;
    if(xoff < 0 || xoff >= vm_map_dim || yoff < 0 || yoff >= vm_map_dim || zoff < 0 || zoff >= vm_column_max) return 0;
    xrel = x - (xoff << 3); yrel = y - (yoff << 3); zrel = z - (zoff << 3);
    column = &map.column[vm_map_dim*yoff + xoff];
    chunk = column->chunk[zoff];
    //printf("xoff,yoff,zoff= %i, %i, %i \n", xoff,yoff,zoff);
    //printf("xrel,yrel,zrel= %i, %i, %i \n", xrel, yrel, zrel);
    if(chunk == NULL) {return 0; printf("t_map _get: chunk null\n");}
    return chunk->voxel[vm_chunk_size*vm_chunk_size*zrel+ vm_chunk_size*yrel + xrel];
}

struct vm_chunk* _get_chunk(int xoff, int yoff, int zoff){
    struct vm_column* column;
    struct vm_chunk* chunk;
    if(xoff < 0 || xoff >= vm_map_dim || yoff < 0 || yoff >= vm_map_dim || zoff < 0 || zoff >= vm_column_max) {
        printf("t_map _get_chunk: parameters out of range\n"); return NULL;
    }
    column = &map.column[vm_map_dim*yoff + xoff];
    chunk = column->chunk[zoff];
    return chunk;
}
///rendering of chunk

//deprecated for server?
int _set_server_version(int x,int y,int z, int server_version) {
    int xoff, yoff, zoff;
    struct vm_column* column;
    struct vm_chunk* chunk;
    xoff = x; yoff = y; zoff = z;
    if(xoff < 0 || xoff >= vm_map_dim || yoff < 0 || yoff >= vm_map_dim || zoff < 0 || zoff >= vm_column_max) {
        printf("t_map _set_server_version: invalid chunk: (%i, %i, %i)\n", x,y,z);
        return 0;
    }
    column = &map.column[vm_map_dim*yoff + xoff];
    chunk = column->chunk[zoff];
    if(chunk == NULL) {
        //printf("creating new chunk \n");
        chunk = new_chunk(xoff, yoff, zoff);
    }
    //column->local_version++;
    chunk->server_version = server_version;
    return 0;
}

int _clear() {
    // iterate entire map
    // set to 0
    int i=0;
    int j=0;
    int k=0;
    for (i=0; i<xmax; i++) {
        for (j=0; j<ymax; j++) {
            for (k=0; k<zmax; k++) {
                _set(i,j,k, 0);
            }
        }
    }
    return 0;
}

int _get_highest_open_block(int x, int y, int n) {
    int i=z_max;
    int open=0;
    int tid=0;

    while (i>0) {
        tid = _get(x,y,i-1);
        if (isSolid(tid)) {
            if (open >= n) {
                break;
            }
            open = 0;
        } else {
            open++;
        }
        i--;
    }
    return i-1;
}

int _get_lowest_open_block(int x, int y, int n) {

}

