
#include "t_map.hpp"

//#include <stdio.h>
//#include <stdlib.h>


#include <c_lib/t_map/cache.hpp>

namespace t_map
{

int init_t_map()
{
#ifdef DC_CLIENT
    init_for_draw();
    init_cache();
#endif

/*
    Data init here
*/
}

int init_for_draw() {

    //init shaders

    //init textures
    init_textures();
    //init cache
    init_cache();

    //_init_quad_cache_normals();

    printf("Terrain map: vertex size is %i bytes \n", sizeof(struct Vertex));


    return 0;
}

struct vm_map map;

struct MapDimension map_dim;

int _init_t_map() {

    int i;
    struct vm_column* c;
    for(i=0; i<vm_map_dim*vm_map_dim; i++) {
        c = &map.column[i];
        c->vbo.v_list = NULL;
        c->vbo.v_list_max_size = 0;
        c->vbo.v_num = 0;
        c->vbo.VBO_id = 0;
        c->x_off = i % vm_map_dim;
        c->y_off = i/vm_map_dim;
        c->flag = 0; //null it
        //c->vbo_needs_update = 0;
        set_flag(c, VBO_loaded, 0);
        set_flag(c, VBO_needs_update, 0);
        set_flag(c, VBO_has_blocks, 0);
    }
    map_dim.x = XMAX;
    map_dim.y = YMAX;
    map_dim.z = ZMAX;
    return 0;
}


/*
zoff and dz are ignored; currently only updates map columns, not map chunks
*/
inline void set_map_chunk_for_update(int xoff, int yoff, int zoff, int dx, int dy, int dz) {
    xoff +=dx;yoff +=dy;
    if(xoff < 0 || xoff >= vm_map_dim || yoff < 0 || yoff >= vm_map_dim ) { return;}
    struct vm_column* column = &map.column[vm_map_dim*yoff + xoff];
    set_flag(column, VBO_needs_update, 1);
}

struct vm_map* _get_map() { return &map; }

struct vm_chunk* new_chunk(int xoff,int yoff,int zoff) {
    int i;
    struct vm_chunk* chunk;
    chunk = (struct vm_chunk*) malloc(sizeof(struct vm_chunk));
    chunk->x_off = xoff;chunk->y_off=yoff;chunk->z_off=zoff;
    chunk->local_version = 0;
    chunk->server_version = 0;
    for(i=0; i<512;i++){
        chunk->voxel[i] = 0;
    }

    return chunk;
}


// terrain map tile set/get

//currently updates map columns, not map chunks!!
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

    if(xrel == 0) set_map_chunk_for_update(xoff,yoff,zoff,-1,0,0);
    if(xrel == 7) set_map_chunk_for_update(xoff,yoff,zoff,1,0,0);
    if(yrel == 0) set_map_chunk_for_update(xoff,yoff,zoff,0,-1,0);
    if(yrel == 7) set_map_chunk_for_update(xoff,yoff,zoff,0,1,0);
    //if(zrel == 0) set_map_chunk_for_update(xoff,yoff,zoff,0,0,-1);
    //if(zrel == 7) set_map_chunk_for_update(xoff,yoff,zoff,0,0,1);

    column = &map.column[vm_map_dim*yoff + xoff];
    chunk = column->chunk[zoff];
    if(chunk == NULL) {
        //printf("creating new chunk \n");
        //printf("xoff,yoff,zoff= %i, %i, %i \n", xoff,yoff,zoff);
        chunk = new_chunk(xoff, yoff, zoff);
        column->chunk[zoff] = chunk;
    }
    chunk->voxel[vm_chunk_size*vm_chunk_size*zrel+ vm_chunk_size*yrel + xrel] = value;
    column->local_version++;
    column->server_version++;
    set_flag(column, VBO_has_blocks, 1);
    chunk->local_version++;
    chunk->server_version++;
    //column->vbo_needs_update = 1;
    set_flag(column, VBO_needs_update, 1);
    set_flag(column, VBO_has_blocks, 1);
    chunk->vbo_needs_update =1;

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
    chunk->requested = 0;
    return chunk;
}
///rendering of chunk

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

/*


*/
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

int get_height_at(int x, int y) {
    int i;
    int c;  // block val
    for (i=ZMAX; i>0; i--) {
        c = _get(x,y,i);
        if (c) return i;
    }
    return 0;
}

inline bool point_in_map(int x, int y, int z)
{
    if (x<0 || x>=map_dim.x || y<0 || y>=map_dim.y || z<0 || z>map_dim.z)
        return false;
    return true;
}

}