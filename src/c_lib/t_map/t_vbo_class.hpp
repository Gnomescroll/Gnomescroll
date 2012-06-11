#pragma once

#include <t_map/glsl/structs.hpp>
#include <t_map/glsl/settings.hpp>

#include "t_map_class.hpp"

#include <physics/quadrant.hpp>

namespace t_map 
{


/*
    Delete VBOs for map chunks outside of a given radius
    Do not update VBOs for map chunks outside of radius
*/

class Map_vbo
{

    static const int MAP_VBO_STARTING_SIZE = 128;

    public:

    float xpos;
    float ypos;

    float xoff;
    float yoff;

    float wxoff;
    float wyoff;

    int vnum;
    int vnum_max;
    struct Vertex* v_list;

    int _v_num[4];
    int _v_offset[4];

    GLuint vbo_id;

    Map_vbo( class MAP_CHUNK* m )
    {

        //flags.flags = 0; //zero all flags

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
        if(vbo_id != 0) glDeleteBuffers(1, &vbo_id);
        delete[] v_list;
    }

};

const int VBO_LIST_SIZE = 1024; //max number of VBOS that can have loaded VBOs


class Vbo_map
{
    const static int CHUNK_IGNORE_DISTANCE2 = (128+32)*(128+32);
    public:    

    class Map_vbo** vbo_array;
    class Terrain_map* map;


    Vbo_map(class Terrain_map* _map)
    {
        map = _map;
        vbo_array = new Map_vbo*[ MAP_CHUNK_XDIM*MAP_CHUNK_YDIM ];
        for(int i=0; i<MAP_CHUNK_XDIM*MAP_CHUNK_YDIM; i++) vbo_array[i] = NULL;
    }

    ~Vbo_map()
    {
        for(int i=0; i<MAP_CHUNK_XDIM*MAP_CHUNK_YDIM; i++) if(vbo_array[i] != NULL) delete vbo_array[i];
        delete[] vbo_array;
    }

    #define MAP_VBO_CULLING 1
    #define MAP_VBO_CULLING_RECYCLE 0   //delete and free vbos out of range
    //update all VBOs that need updating
    void update_map()
    {
    #if MAP_VBO_CULLING
        int cx = (int) current_camera_position.x;
        int cy = (int) current_camera_position.y;
    #endif

        class MAP_CHUNK* m;

        for(int i=0; i<MAP_CHUNK_XDIM; i++)
        for(int j=0; j<MAP_CHUNK_YDIM; j++)
        {
            const int index = j*MAP_CHUNK_XDIM + i;
            m = map->chunk[index];
            if( m == NULL ) continue; //can speed up by maintain list of chunks

        #if MAP_VBO_CULLING
            int x = cx - quadrant_translate_i(cx, 16*i+8 );
            int y = cy - quadrant_translate_i(cy, 16*j+8 );
            int distance2 = x*x+y*y;

            if(distance2 >= CHUNK_IGNORE_DISTANCE2)
            {

            #if MAP_VBO_CULLING_RECYCLE
                if( vbo_array[index] != NULL )
                {
                    //chunk vbo is out of radius and is loaded
                    //delete
                    delete vbo_array[index]; 
                    vbo_array[index] = NULL;
                    m->needs_update == true;    //so it gets recreated if in range
                    continue;
                }
            #endif
                //chunk vbo is out of radius and is not loaded, do nothing
                continue;

            }
        #endif

            if( m->needs_update == false ) continue;
            m->needs_update = false; //reset flag
            if( vbo_array[index] == NULL ) vbo_array[index] = new Map_vbo( m );
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

    }

    void update_vbo(int i, int j);
    void update_vbo_comptability(int i, int j);

    void prep_draw();
    void sort_draw();

    void draw_map();
    void draw_map_comptability();
};

}