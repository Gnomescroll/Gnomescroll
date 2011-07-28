

#DEFINE vm_map_dim 64 //number of map chunks in x/y
#DEFINE vm_chunk_size 8
#DEFINE vm_column_max 16

struct vm_chunk {
    ushort voxel[512];
    int x_off, y_off, z_off; //lower corner of chunk
    uint local_version;
    uint server_version;
};

struct vm_column {
    int x_off, y_off;
    struct vm_chunk* chunk[vm_column_max];
    uint local_version;
    uint server_version;
    //vm_column_history history;
};

struct vm_map {
    struct vm_column column[ma_dim*ma_dim];
};

int init_t_map();
int _set(int x, int y, int z, int value);
int _get(int x, int y, int z);
struct vm_map* _get_map();
struct vm_chunk* _get_chunk(int xoff, int yoff, int zoff);
