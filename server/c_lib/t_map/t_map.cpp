#include "t_map.hpp"

#include <zlib.h>
#include "t_properties.h"

#include <stdio.h>
#include <stdlib.h>

#include <c_lib/common/functional.h>

#include <t_map/t_map_net.hpp>

//#include <net_lib/net.hpp>

/*
    Non-networking
*/
struct vm_map map;

struct MapDimension map_dim;

int _init_t_map() {
    printf("MAP SIZE %d\n", sizeof(map));
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
    map_dim.x = XMAX;
    map_dim.y = YMAX;
    map_dim.z = ZMAX;
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

// destroys block and returns 0 if damage exceeds max damage
// else return current damage of block
// return  -4 when damage <= 0 (would have had no or bad effects)
// returns -3 when chunk is NULL
// returns -2 on map coordinates out of bounds failure
// returns -1 when  blocks is already 0 / empty
int _apply_damage(int x, int y, int z, int dmg) {
    if (dmg <= 0) return -4;
    int xoff, yoff, zoff, xrel, yrel, zrel;
    int tile;
    struct vm_column* column;
    struct vm_chunk* chunk;
    xoff = x >> 3; yoff = y >> 3; zoff = z >> 3;
    if(xoff < 0 || xoff >= vm_map_dim || yoff < 0 || yoff >= vm_map_dim || zoff < 0 || zoff >= vm_column_max) {
        return -2;
    }
    xrel = x - (xoff << 3); yrel = y - (yoff << 3); zrel = z - (zoff << 3);
    column = &map.column[vm_map_dim*yoff + xoff];
    chunk = column->chunk[zoff];
    if(chunk == NULL) {
        //printf("_apply_damage: Chunk is NULL:: %d,%d,%d dmg=%d\n", x,y,z, dmg);
        //printf("xyzoff:: %d %d %d\n", xoff, yoff, zoff);
        return -3;
    }
    tile = chunk->voxel[vm_chunk_size*vm_chunk_size*zrel+ vm_chunk_size*yrel + xrel];
    if (tile==0) {
        return -1;
    }
    unsigned char* damage;
    damage = &chunk->damage[vm_chunk_size*vm_chunk_size*zrel+ vm_chunk_size*yrel + xrel];
    *damage += dmg;
    if(*damage >= _maxDamage(tile) ) {
        // destroy block
        _set(x,y,z, 0);
        return 0;
    } else {
        return *damage;
    }
}

// apply block damage & broadcast the update to client
int _apply_damage_broadcast(int x, int y, int z, int dmg) {
    int res = _apply_damage(x,y,z, dmg);
    if (res == 0) {
        _block_broadcast(x,y,z, 0);
    }
    return res;
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

// set a chunk's voxels in bulk
int _set_chunk_voxels(int xoff, int yoff, int zoff, unsigned short* vox) {

    if(xoff < 0 || xoff >= vm_map_dim || yoff < 0 || yoff >= vm_map_dim || zoff < 0 || zoff >= vm_column_max) {
        printf("Warning:  _set_chunk_voxels :: chunk offsets out of range :: %d %d %d\n", xoff, yoff, zoff);
        return 1;
    }
        
    struct vm_column* column;
    struct vm_chunk* chunk;

    column = &map.column[vm_map_dim*yoff + xoff];
    chunk = column->chunk[zoff];

    if (chunk == NULL) {
        chunk = new_chunk(xoff, yoff, zoff);
        column->chunk[zoff] = chunk;
    }
    
    int i,j=0;
    for (i=0; i < vm_chunk_voxel_size; i++) {
        chunk->voxel[i] = vox[i];
        if (vox[i] != 0) j++;
    }
    printf("NONEMPTY VOXELS THIS CHUNK: %d\n", j);
    return 0;
}

// set a chunk voxel, using chunk offsets and voxel array index
int _set_chunk_voxel(int xoff, int yoff, int zoff, unsigned short val, int i) {

    if(xoff < 0 || xoff >= vm_map_dim || yoff < 0 || yoff >= vm_map_dim || zoff < 0 || zoff >= vm_column_max) {
        printf("Warning:  _set_chunk_voxel :: chunk offsets out of range :: %d %d %d\n", xoff, yoff, zoff);
        return 1;
    }

    if (i < 0 || i >= vm_chunk_voxel_size) {
        printf("Warning: _set_chunk_voxel :: voxel array index out of range (%d) :: %d\n", vm_chunk_voxel_size, i);
        return 1;
    }
        
    struct vm_column* column;
    struct vm_chunk* chunk;

    column = &map.column[vm_map_dim*yoff + xoff];
    chunk = column->chunk[zoff];

    if (chunk == NULL) {
        chunk = new_chunk(xoff, yoff, zoff);
        column->chunk[zoff] = chunk;
    }

    chunk->voxel[i] = val;
    //chunk->voxel[i] = 1;
    //if (val) printf("set chunk voxel to %u\n", val);
    return 0;
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
    int i,j,k;
    for (i=0; i<XMAX; i++) {
        for (j=0; j<YMAX; j++) {
            for (k=0; k<ZMAX; k++) {
                _set(i,j,k, 0);
            }
        }
    }
    return 0;
}
int _get_highest_open_block(int x, int y, int n) {
    if (n < 1) {
        printf("WARNING: _get_highest_open_block :: called with n < 1\n");
        return -1;
    }
    if (n==1) return _get_highest_solid_block(x,y) + 1;

    int open=n;
    int block;
    int i;

    for (i=ZMAX-1; i>=0; i--) {
        block = _get(x,y,i);
        if (!isSolid(block)) {
            open++;
        } else {
            if (open >= n) {
                return i+1;
            }
            open = 0;
        }
    }
    if (open >= n) return 0;
    return -1;
}

int _get_highest_solid_block(int x, int y) {

    int i;
    for (i=ZMAX-1; i>=0; i--) {
        if (isSolid(_get(x,y,i))) {
            break;
        }
    }
    return  i;
}

int _get_lowest_open_block(int x, int y, int n) {
    if (n < 1) {
        printf("WARNING: _get_lowest_open_block :: called with n < 1\n");
        return -1;
    }

    int i;
    int block;
    int open=0;
    for (i=0; i<ZMAX; i++) {
        block = _get(x,y,i);
        if (isSolid(block)) {
            open = 0;
        } else {
            open++;
        }
        if (open >= n) return i-open+1;
    }

    return i;
}

int _get_lowest_solid_block(int x, int y) {

    int i;
    for (i=0; i < ZMAX; i++) {
        if (isSolid(_get(x,y,i))) {
            break;
        }
    }
    if (i >= ZMAX) i = -1;  // failure
    return i;
}

int _set_broadcast(int x, int y, int z, int value) {
    block_StoC msg;
    int i,j=0;
    i = _get(x,y,z);
    if (i != value) {
        j = _set(x,y,z, value);
        msg.x = x;
        msg.y = y;
        msg.z = z;
        msg.val = value;
        msg.broadcast();
    }
    return j;
}

void _block_broadcast(int x, int y, int z, int value) {
    block_StoC msg;
    msg.x = x;
    msg.y = y;
    msg.z = z;
    msg.val = value;
    msg.broadcast();
}

void send_map_metadata(int client_id)
{
    map_metadata_StoC msg;
    msg.x = map_dim.x;
    msg.y = map_dim.y;
    msg.z = map_dim.z;
    msg.sendToClient(client_id);
}

void send_map_metadata()
{
    map_metadata_StoC msg;
    msg.x = map_dim.x;
    msg.y = map_dim.y;
    msg.z = map_dim.z;
    msg.broadcast();
}

void set_map_size(int x, int y, int z)
{
    map_dim.x = x;
    map_dim.y = y;
    map_dim.z = z;
    send_map_metadata();
}
