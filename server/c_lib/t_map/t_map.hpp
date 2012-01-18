#pragma once



#define vm_map_dim 64 //number of map chunks in x/y
#define vm_chunk_size 8
#define vm_column_max 16
#define vm_chunk_voxel_size (vm_chunk_size * vm_chunk_size * vm_chunk_size)

#define xmax (vm_map_dim    * vm_chunk_size)
#define ymax (vm_map_dim    * vm_chunk_size)
#define zmax (vm_column_max * vm_chunk_size)


struct VBO {
        int v_num;
        struct Vertex* v_list;
        int VBO_id;
};

struct vm_chunk {
    unsigned short voxel[vm_chunk_voxel_size];
    unsigned char damage[vm_chunk_voxel_size];
    int x_off, y_off, z_off; //lower corner of chunk
    unsigned int local_version;
    unsigned int server_version;
    unsigned int vbo_needs_update;
};

struct vm_column {
    int x_off, y_off;
    struct vm_chunk* chunk[vm_column_max];
    unsigned int local_version;
    unsigned int server_version;
    //vm_column_history history;
    //unsigned int vbo_needs_update;
    //unsigned int vbo_loaded;
    //unsigned int empty; // has blocks
    unsigned int flag;
    struct VBO vbo;
};

struct vm_map {
    struct vm_column column[vm_map_dim*vm_map_dim];
};


//functions
extern struct vm_map map;

int _init_t_map();
int _set(int x, int y, int z, int value);
int _get(int x, int y, int z);
int _clear();

int _apply_damage(int x, int y, int z, int dmg);

int _set_server_version(int x,int y,int z, int server_version);

struct vm_map* _get_map();
struct vm_chunk* _get_chunk(int xoff, int yoff, int zoff);

int _set_chunk_voxels(int xoff, int yoff, int zoff, unsigned short* vox);
int _set_chunk_voxel(int xoff, int yoff, int zoff, unsigned short val, int i);

int _get_highest_open_block(int x, int y, int n=1);
int _get_lowest_open_block(int x, int y, int n=1);

int _get_highest_solid_block(int x, int y);
int _get_lowest_solid_block(int x, int y);

int _set_broadcast(int x, int y, int z, int value);

//these flags are not used on server for anything
/*
#define VBO_loaded 1
#define VBO_needs_update 2
#define VBO_has_blocks 4
#define VBO_drawn 8
*/

static inline void set_flag(struct vm_column* c, int flag, int value) {
    if(value) {
        c->flag = c->flag | flag;
    } else {
        c->flag = c->flag & ~flag;
    }
}

static inline int flag_is_true(struct vm_column* c, int flag) {
    if((c->flag & flag) == 0) { return 0; } else {return 1;}
}

static inline int flag_is_false(struct vm_column* c, int flag) {
    if((c->flag & flag) == 0) { return 1; } else {return 0;}
}


void _block_broadcast(int x, int y, int z, int value);
int _apply_damage_broadcast(int x, int y, int z, int dmg);


