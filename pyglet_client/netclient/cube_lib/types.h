
#ifndef TYPESh

#define TYPESh

//map data structures

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

// vbo stuff
struct Vertex {
    float x,y,z;
    float tx,ty;
    unsigned char r,g,b,a;
};

struct VBO {
    int v_num;
    struct Vertex* v_list;
    int VBO_id;
};


#endif
