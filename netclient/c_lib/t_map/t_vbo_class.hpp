#pragma once

#include <c_lib/t_map/glsl/structs.hpp>
#include <c_lib/t_map/glsl/settings.hpp>

#include "t_map_class.hpp"

//#include <stdio.h>

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

const int MAP_VBO_STARTING_SIZE = 128;
const int MAP_VBO_INCREMENT = 128;

struct VBO_FLAGS
{
    union
    {
        struct
        {
            bool vertex_list_loaded;
            bool vbo_loaded;
            bool render;   
        };
        int flags;
    };
};

class Map_vbo
{
    public:
    //flags
    struct VBO_FLAGS flags;

    float xpos;
    float ypos;

    float xoff;
    float yoff;

    int vnum;
    int vnum_max;
    struct Vertex* v_list;

    int _v_num[4];
    int _v_offset[4];

    GLuint vbo_id;

    explicit Map_vbo( class MAP_CHUNK* m )
    {

        flags.flags = 0; //zero all flags

        xpos = m->xpos + 8.0;
        ypos = m->ypos + 8.0;

        xoff = m->xpos;
        yoff = m->ypos;

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

};

const int VBO_LIST_SIZE = 1024; //max number of VBOS that can have loaded VBOs

class Vbo_map
{
    public:    

    int xchunk_dim;
    int ychunk_dim;

    class Map_vbo** vbo_array;
    class Terrain_map* map;

    class Map_vbo** vbo_list; //list of loaded vbos

    explicit Vbo_map(class Terrain_map* _map)
    {
        map = _map;
        xchunk_dim = _map->xchunk_dim;
        ychunk_dim = _map->ychunk_dim;
        vbo_array = new Map_vbo*[ xchunk_dim*ychunk_dim ];
        for(int i=0; i<xchunk_dim*ychunk_dim; i++) vbo_array[i] = NULL;

        vbo_list = new Map_vbo*[VBO_LIST_SIZE];
        for(int i=0; i<VBO_LIST_SIZE; i++) vbo_list[i] = NULL;
    }

    ~Vbo_map()
    {
        for(int i=0; i<xchunk_dim*ychunk_dim; i++) if(vbo_array[i] != NULL) delete vbo_array[i];
        delete[] vbo_array;
    }

/*
    void clean_vbos()
    {
        
        for(int i=0; i<xchunk_dim; i++)
        for(int j=0; j<ychunk_dim; j++)
        {
            if( vbo_array[j*xchunk_dim + i] == NULL ) continue;

            if( )
        }
    }
*/

    //update all VBOs that need updating
    void update_map()
    {
        //int count = 0;

        class MAP_CHUNK* m;
        for(int i=0; i<xchunk_dim; i++)
        for(int j=0; j<ychunk_dim; j++)
        {

            m = map->chunk[j*xchunk_dim + i];
            if( m == NULL ) continue; //can speed up by maintain list of chunks

            if( m->needs_update == false ) continue;
            m->needs_update = false; //reset flag

            //count++;
            if( vbo_array[j*xchunk_dim + i] == NULL ) vbo_array[j*xchunk_dim + i] = new Map_vbo( m );
            //printf("updating vbo: %i %i \n", i, j);
            
            if(T_MAP_BACKUP_SHADER == 0)
            {
                update_vbo(i, j);
            }
            else
            {
                update_vbo_comptability(i, j);
            }
            return;
        }

        //if(count > 0) printf("!!! count= %i \n", count);
    }

    void update_vbo(int i, int j);
    void update_vbo_comptability(int i, int j);

    void prep_draw();
    void draw_map();
    void draw_map_comptability();
};

}
