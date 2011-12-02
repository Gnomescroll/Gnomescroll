#include "./t_properties.h"

struct cubeProperties cube_list[max_cubes];

short infinite_texture_array[1024];

short cube_side_texture_array[max_cubes*6];

void set_infinite_texture(int id, int texture) {
    if(id > 1024) {
        printf("ERROR: more than 1024 infinite texture tiles, increase array size\n");
    }
    infinite_texture_array[id] = texture;
    //printf("id=%i texture= %i \n", id, texture);
}

int get_infinite_texture(int index) {
    return infinite_texture_array[index];
}

void init_t_properties() {
    int i;
    for(i=0;i<max_cubes; i++) cube_list[i].infinite_texture = 0;
    for(i=0;i<max_cubes*6; i++) cube_side_texture_array[i] = 255; //use error block
}

struct cubeProperties* _get_cube_list() {
    return cube_list;
}

struct cubeProperties* _get_cube(int id) {
    return &cube_list[id];
}

int _isActive(int id) {
    return cube_list[id].active;
    }


void _set_cube_side_texture(int id, int side, int tex_id) {
    
    if(6*id +side > max_cubes)
    {
        printf("_set_cube_side_texture: error, would overflow array\n");
        return;
    }
    cube_side_texture_array[6*id +side] = tex_id;
}
/*
int _get_cube_side_texture(int id, int side)
{
    return cube_side_texture_array[6*id +side];
}
*/