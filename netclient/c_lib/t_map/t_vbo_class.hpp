#pragma once

#include <c_lib/t_map/glsl/structs.hpp>

#include "t_map_class.hpp"

#include <stdio.h>

namespace t_map 
{

struct column
{
    int offset;
    int vnum;
    unsigned short s[TERRAIN_MAP_HEIGHT]; //num vertices in slice
    //unsigned short o[TERRAIN_MAP_HEIGHT]; //offset of vertices in slice, for drawing
};

/*
    Iterate over slices, copying in chunks until something needs to be updated, update, insert and continue
*/

const int MAP_VBO_STARTING_SIZE = 1024;
const int MAP_VBO_INCREMENT = 256;

class Map_vbo
{
    public:

    int vnum;
    int vnum_max;
    struct Vertex* v_list;

    float xpos;
    float ypos;

    int _v_num[4];
    int _v_offset[4];

    GLuint vbo_id;

    Map_vbo( class MAP_CHUNK* m )
    {
        xpos = m->xpos + 8.0;
        ypos = m->ypos + 8.0;

        v_list = new Vertex[MAP_VBO_STARTING_SIZE];
        vnum_max = MAP_VBO_STARTING_SIZE;
        vnum = 0;
        vbo_id = 0;
    }

    ~Map_vbo()
    {
        delete[] v_list;
    }

    void resize(int num)
    {
        delete[] v_list;
        vnum_max = (num - (num % MAP_VBO_INCREMENT)) + MAP_VBO_INCREMENT;
        v_list = new Vertex[ vnum_max ];
    }

    /*
        Has to copy to new buffer, cannot use
    */

    /*
    void expand(int num) //vertex num currently being processed
    {
        vnum_max += MAP_VBO_INCREMENT;
        struct Vertex* v_tmp= new Vertex[ vnum_max ];
        memcpy(v_tmp, v_list, num*sizeof(struct Vertex));
        delete[] v_list;
    }
    */

};

class Vbo_map
{
    public:    

    int xchunk_dim;
    int ychunk_dim;

    class Map_vbo** vbo_array;
    class Terrain_map* map;

    Vbo_map(class Terrain_map* _map)
    {
        map = _map;
        xchunk_dim = _map->xchunk_dim;
        ychunk_dim = _map->ychunk_dim;
        vbo_array = new Map_vbo*[ xchunk_dim*ychunk_dim ];
        for(int i=0; i<xchunk_dim*ychunk_dim; i++) vbo_array[i] = NULL;
    }

    ~Vbo_map()
    {
        for(int i=0; i<xchunk_dim*ychunk_dim; i++) if(vbo_array[i] != NULL) delete vbo_array[i];
        delete[] vbo_array;
    }

    //update all VBOs that need updating
    void update_map()
    {
        class MAP_CHUNK* m;
        for(int i=0; i<xchunk_dim; i++)
        for(int j=0; j<ychunk_dim; j++)
        {
            m = map->chunk[j*xchunk_dim + i];
            if( m == NULL ) continue; //can speed up by maintain list of chunks

            //printf("t\n");

            if( m->needs_update == false ) continue;
            m->needs_update = false; //reset flag

            if( vbo_array[j*xchunk_dim + i] == NULL ) vbo_array[j*xchunk_dim + i] = new Map_vbo( m );
            //printf("updating vbo: %i %i \n", i, j);
            update_vbo(i, j);

        }
    }

    void update_vbo(int i, int j);
    
    void prep_draw();
    void draw_map();

};

}