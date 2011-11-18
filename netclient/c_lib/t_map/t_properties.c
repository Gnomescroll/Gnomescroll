#include "./t_properties.h"

struct cubeProperties cube_list[max_cubes];

short infinite_texture_array[1024];

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
