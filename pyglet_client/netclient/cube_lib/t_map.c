
int init_t_map();
int _set(int x, int y, int z, int value);
int _get(int x, int y, int z);

#define vm_map_dim 64 //number of map chunks in x/y
#define vm_chunk_size = 8
#define vm_column_max = 16

//chunks

struct vm_chunk {
    ushort voxel[512];
    int x_off, y_off, z_off; //lower corner of chunk
    uint local_version;
    uint server_version;
};

struct vm_column {
    int x_off, y_off;
    vm_chunk* chunk[vm_column_max];
    uint local_version;
    uint server_version;
    //vm_column_history history;
}

struct vm_map {
    vm_column* column[ma_dim*ma_dim]
}

//globals

vm_map map;

int init_t_map() {
    printf("size of vm_chunk_list= %i, should be 520", sizeof(vm_chunk_list));
    printf("size of vm_chunk_status= %i, should be 8", sizeof(vm_chunk_status));
    return 0;
}

// terrain map tile set/get
int _set(int x, int y, int z, int value){
    int xoff, yoff, zoff, xrel, yrel, zrel;
    vm_column* column;
    vm_chunk* chunk;
    xoff = x >> 3; yoff = y >> 3; zoff = z >> 3;
    if(xoff < 0 || xoff >= vm_map_dim || yoff < 0 || yoff >= vm_map_dim || zoff < 0 || vm_column_max >= vm_column_max) {
        printf("t_map set: invalid Assignment to map: (%i, %i, %i, %i)", x,y,z,value);
        return 0;
    }
    xrel = x - (xoff << 3); yrel; = y - (yoff << 3); zrel = z - (zoff) << 3);
    column = &map.chunk[vm_map_dim*yoff + xoff];
    chunk = column[zoff];
    if(chunk == NULL) {
        column[zoff] = (vm_chunk*)malloc(sizeof(vm_chunk));
        chunk = column[zoff];
    }
    chunk->voxel[vm_chunk_size*vm_chunk_size*zrel+ vm_chunk_size*yrel + xrel] = value;
    column->local_version++;
    chunk->local_version++;
    return 0;
}

int _get(int x, int y, int z) {
    int xoff, yoff, zoff;
    vm_column* column;
    vm_chunk* chunk;
    xoff = x >> 3; yoff = y >> 3; zoff = z >> 3;
    if(xoff < 0 || xoff >= vm_map_dim || yoff < 0 || yoff >= vm_map_dim || zoff < 0 || vm_column_max >= vm_column_max) return 0;
    xrel = x - (xoff << 3); yrel; = y - (yoff << 3); zrel = z - (zoff) << 3);
    column = &map.chunk[vm_map_dim*yoff + xoff];
    chunk = column.chunk[zoff];
    if(chunk == NULL) return 0;
    return chunk->voxel[vm_chunk_size*vm_chunk_size*zrel+ vm_chunk_size*yrel + xrel];
}


///WTF

//chunk messages
struct vm_chunk_status { //8 bytes
    ushort x_off, y_off, z_off;
    ushort server_version;
};

//576 is the largest MTU that is guaranteed never to require fragmentation
struct vm_chunk_list { //512+8 bytes
    uint number;
    vm_chunk_status array[64]; //
};

struct vm_column_status {


}

struct vm_column_list {

}

int _send_chunk_list(int client_id, void* callback) {


}
//
//fulstrum culling

int _set_camera(
    float distance, float x_view_angle, float y_view_angle,
    float x0,float y0,float z0, //viewing direction,
    float x1,float y1,float z1,
    float x2,float y2,float z2) {


}
