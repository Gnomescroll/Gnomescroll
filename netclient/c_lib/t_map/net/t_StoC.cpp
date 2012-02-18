#include "t_StoC.hpp"

#include "t_CtoS.hpp"

#include "../t_map.hpp"

#ifdef DC_CLIENT

#include <c_lib/animations/animations.hpp>
#include <c_lib/common/random.h>
#include <sound/triggers.hpp>

using namespace t_map;

int client_chunk_alias_list[1024] = {0};



//unsigned short chunk_alias;
//int chunk_index;
//int byte_size;
void map_chunk_uncompressed_StoC::handle(char* buff, int buff_n, int* bytes_read, int max_n)
{
    printf("map_chunk: alias= %i for %i %i \n", chunk_alias, chunk_index%MAP_CHUNK_WIDTH, chunk_index /MAP_CHUNK_WIDTH );
    printf("byte_size= %i \n", byte_size);
}


//unsigned short chunk_alias;
//unsigned int chunk_index;
void set_map_alias_StoC::handle()
{
    client_chunk_alias_list[chunk_alias] = chunk_index;
    printf("Alias %i set to %i %i \n", chunk_alias, chunk_index%MAP_CHUNK_WIDTH, chunk_index /MAP_CHUNK_WIDTH);
}

void send_map_chunk(int x, int y, char* buffer, int n) {}

void handle_map_chunk(int x, int y, char *buffer, int n)
{
    
}

//unsigned short chunk_alias;
//unsigned short chunk_version; //for debugging
void end_map_stream::handle()
{
    printf("received stream end \n");
}

//unsigned short chunk_alias;
//unsigned char block_id;
//unsigned char pallete;

void map_element_update::handle()
{
    printf("received map element update \n");
}

/*
    Old Methods
*/
void chunk_meta_data_StoC::handle()
{
    //int chunk_x,chunk_y;
    //int version;
    printf("chunk_meta_data_StoC: chunk_x= %i chunk_y= %i version= %i \n", chunk_x, chunk_y, version);
}

void block_StoC::handle() 
{
    if (val == 0) 
    {
        int cube_id = _get(x,y,z);
        Animations::block_crumble((float)x+0.5f, (float)y+0.5f, (float)z+0.5f, randrange(10,30), cube_id);
        Sound::block_destroyed(x, y, z, 0,0,0);
    }
    else
        Sound::block_set(x,y,z,0,0,0);
    _set(x,y,z,val);
}

void map_metadata_StoC::handle() 
{
    map_dim.x = x;
    map_dim.y = y;
    map_dim.z = z;
}


#endif


#ifdef DC_SERVER

void map_chunk_uncompressed_StoC::handle(char* buff, int buff_n, int* bytes_read, int max_n) {}

void set_map_alias_StoC::handle() {}

void chunk_meta_data_StoC::handle() {}
void block_StoC::handle() {}
void map_metadata_StoC::handle() {}

void handle_map_chunk() {}

void send_map_chunk() {}

#endif
