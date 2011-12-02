#include "./t_properties.h"

#include <stdio.h>
#include <stdlib.h>

#include "t_vbo.h"

struct cubeProperties cube_list[max_cubes];

short infinite_texture_array[1024];

static short cube_side_texture_array[max_cubes*6];
static unsigned char* pixel_data[MAX_TEXTURES];

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
    //for(i=0;i<MAX_TEXTURES; i++) pixel_data[i] = NULL;
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

/*
Texture stuff

*/

int _get_cube_side_texture(int id, int side)
{
    return cube_side_texture_array[6*id +side];
}


const static int TEXTURE_WIDTH = 32;

void _set_cube_side_texture(int id, int side, int tex_id) {
    if(tex_id < 0 ) return;

    if(tex_id > MAX_TEXTURES)
    {
        printf("_set_cube_side_texture: error, tex id would exceed MAX_TEXTURES: id= %i, side=%i, tex_id= %i\n", id, side, tex_id);
        return;
    }
    if(6*id + side > max_cubes*6)
    {
        printf("_set_cube_side_texture: error, would overflow array: id= %i, side=%i, tex_id= %i\n", id, side, tex_id);
        return;
    }
    cube_side_texture_array[6*id +side] = tex_id;

    printf("tex_id= %i \n", tex_id);

    //init pixel sampler if it has not been inited
    if(pixel_data[tex_id] == NULL) 
    {
        pixel_data[tex_id] = new unsigned char[4*32*32];

        int _tx = tex_id % 16;
        int _ty = tex_id / 16;
        
        printf("tx, ty= %i, %i \n", _tx, _ty);
                
        int tx = _tx*32;
        int ty = _ty*32;
        
        int px, py;
        unsigned char r,b,g,a;

        for(int i=0; i < 32; i++) {
        for(int j=0; j < 32; j++) {
            //pixel offset
            px = tx + i;
            py = ty + j;
            //get_texture_pixel(int px, int py, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a);
            //get RGBA value
            get_texture_pixel(px, py, &r, &g,&b, &a);
            
            pixel_data[tex_id][4*(32*i+j)+0] = r;
            pixel_data[tex_id][4*(32*i+j)+1] = g;
            pixel_data[tex_id][4*(32*i+j)+2] = b;
            pixel_data[tex_id][4*(32*i+j)+3] = a;
        }
        }
    }
}


void get_random_pixel(int cube_id, int side, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a)
{
    int tex_id = _get_cube_side_texture(cube_id, side);
    printf("tex_id= %i\n", tex_id);

    int ra = rand() % (32*32);
    *r = pixel_data[tex_id][4*(ra)+0];
    *g = pixel_data[tex_id][4*(ra)+1];
    *b = pixel_data[tex_id][4*(ra)+2];
    *a = pixel_data[tex_id][4*(ra)+3];
}

void get_texture_pixel(int px, int py, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a) {
    Uint32 pixel = px + py*block_surface_width;
    SDL_GetRGBA(pixel, sdl_pixel_format, r,g,b,a);
}

