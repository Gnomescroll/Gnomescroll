#pragma once

#include <t_map/glsl/structs.hpp>
#include <t_map/glsl/settings.hpp>
#include <t_map/common/map_element.hpp>

#include "t_map_class.hpp"

#include <physics/quadrant.hpp>

namespace t_map 
{


/*
    Delete VBOs for map chunks outside of a given radius
    Do not update VBOs for map chunks outside of radius
    Put VBO refreshes on a que and sort by distance, recency and viewing fulstrum
    Thead VBO generation
    Double buffer VBOs
*/

/*
    Improve viewing fulstrum test
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

    int vertex_num[6];
    int vertex_offset[6];

    //int vertex_num_array[7][16];   //for each column, every 8 z
    int voff_array[6][8+1]; //levels

    GLuint vbo_id;

    Map_vbo( class MAP_CHUNK* m )
    {

        //flags.flags = 0; //zero all flags

        xpos = m->xpos + 8.0f;
        ypos = m->ypos + 8.0f;

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
    const static int CHUNK_IGNORE_DISTANCE2 = (128+64)*(128+64);
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
    #define MAP_VBO_CULLING_RECYCLE 1   //delete and free vbos out of range
    //update all VBOs that need updating

/*
    Dont load chunk until its neightbors are loaded!
*/
    void update_map()
    {
    #if MAP_VBO_CULLING
        int cx = (int) current_camera_position.x;
        int cy = (int) current_camera_position.y;
    #endif

        class MAP_CHUNK* m;
        int min_distance2 = 0x7FFFFFF;
        int _i = -1;
        int _j = -1;

        //find closest chunk to update
        for(int i=0; i<MAP_CHUNK_XDIM; i++)
        for(int j=0; j<MAP_CHUNK_YDIM; j++)
        {
            const int index = j*MAP_CHUNK_XDIM + i;
            m = map->chunk[index];
            if( m == NULL ) continue; //can speed up by maintain list of chunks

            int x = cx - quadrant_translate_i(cx, 16*i+8 );
            int y = cy - quadrant_translate_i(cy, 16*j+8 );
            int distance2 = x*x+y*y;

        #if MAP_VBO_CULLING

            if(distance2 >= CHUNK_IGNORE_DISTANCE2)
            {

            #if MAP_VBO_CULLING_RECYCLE
                if( vbo_array[index] != NULL )
                {
                    //chunk vbo is out of radius and is loaded
                    //delete
                    delete vbo_array[index]; 
                    vbo_array[index] = NULL;
                    m->needs_update = true;    //so it gets recreated if in range

                    //printf("deleting: %i \n", index);
                    continue;
                }
            #endif
                //chunk vbo is out of radius and is not loaded, do nothing
                continue;

            }
        #endif

            if( m->needs_update == false ) continue;

            if(distance2 < min_distance2)
            {
                min_distance2 = distance2;
                _i = i;
                _j = j;
            }

        }
        //update if any chunks in range need updating
        if(_i != -1)
        {
            const int index = _j*MAP_CHUNK_XDIM + _i;
            //printf("updating index: %i \n", index);

            m = map->chunk[index];

            m->needs_update = false; //reset flag
            if( vbo_array[index] == NULL ) vbo_array[index] = new Map_vbo( m );
            //printf("updating vbo: %i %i \n", i, j);
            
            if(T_MAP_BACKUP_SHADER == 0)
            {
                update_vbo(_i, _j);
            }
            else
            {
                update_vbo_compatibility(_i, _j);
            }
            return;
        }

    }

    //for VBO projection
    void load_all()
    {
        for(int i=0; i<MAP_CHUNK_XDIM; i++)
        for(int j=0; j<MAP_CHUNK_YDIM; j++)
        {
            const int index = j*MAP_CHUNK_XDIM + i;
            class MAP_CHUNK* m = map->chunk[index];
            if( m == NULL ) continue; //can speed up by maintain list of chunks
            if( m->needs_update == false ) continue;
            m->needs_update = false; //reset flag
            if( vbo_array[index] == NULL ) vbo_array[index] = new Map_vbo( m );
            update_vbo(i, j);
        }

    }


    void update_vbo(int i, int j);
    void update_vbo_compatibility(int i, int j);

    void prep_draw();
    void sort_draw();

    void draw_map();
    void draw_map_compatibility();

    void prep_frustrum();
    void prep_frustrum_vertices();
};


}