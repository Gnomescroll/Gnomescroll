#include "t_StoC.hpp"

#include "t_CtoS.hpp"

#include "../t_map.hpp"
//#include "../t_map_class.hpp"

namespace t_map
{

#ifdef DC_CLIENT

#include <c_lib/animations/animations.hpp>
#include <c_lib/common/random.h>
#include <sound/triggers.hpp>


int client_chunk_alias_list[1024] = {0};

mz_stream stream;

const int DECOMPRESSION_BUFFER_SIZE = 1024*512;
char* DECOMPRESSION_BUFFER = NULL;

#define MAP_NET_DEBUG 0
//unsigned short chunk_alias;
//int chunk_index;

int  init_compressors()
{
    // Init the z_stream
    DECOMPRESSION_BUFFER = (char*) malloc(DECOMPRESSION_BUFFER_SIZE);

    memset(&stream, 0, sizeof(stream));

    if (mz_inflateInit(&stream))
    {
        printf("map_chunk_compressed_StoC::handle, inflateInit() failed!\n");
        return 0;
    }

    return 1;
}

void map_chunk_compressed_StoC::handle(char* buff, int byte_num)
{

    static int init = 0;
    if(init == 0) init = init_compressors();


    stream.next_in = (unsigned char*) buff;
    stream.avail_in = byte_num;

    stream.next_out = (unsigned char*) DECOMPRESSION_BUFFER;
    stream.avail_out = DECOMPRESSION_BUFFER_SIZE;

    int status = mz_inflate(&stream, MZ_SYNC_FLUSH);

    if (status != MZ_OK)
    {
        printf("inflate() failed with status %i!\n", status);
        return;
    }


    int size = DECOMPRESSION_BUFFER_SIZE - stream.avail_out;

    //printf("compressed chunk: %i bytes decompressed to %i bytes \n", byte_num, size);
    
    /*
        Handle
    */

    client_chunk_alias_list[chunk_alias] = chunk_index;

    int x = chunk_index % MAP_CHUNK_WIDTH;
    int y = chunk_index / MAP_CHUNK_WIDTH;
    
    struct MAP_CHUNK* m = main_map->chunk[chunk_index];
    if(m == NULL)
    {
        main_map->set_block(16*x,16*y,0, 0); //create chunk    
        m = main_map->chunk[chunk_index];
    }


    int _size = sizeof(struct MAP_ELEMENT)*16*16*TERRAIN_MAP_HEIGHT;

    if(size != _size) printf("map_chunk_compressed_StoC::handle, warning: invalid size!\n");

    memcpy( (char *) m->e, DECOMPRESSION_BUFFER, _size);

}

void map_chunk_uncompressed_StoC::handle(char* buff, int byte_num)
{
    //printf("map_chunk: alias= %i for %i %i \n", chunk_alias, chunk_index%MAP_CHUNK_WIDTH, chunk_index /MAP_CHUNK_WIDTH );
    //printf("byte_size= %i \n", byte_size);
#if MAP_NET_DEBUG
    printf("map chunk is %i bytes \n", byte_size);
#endif

    client_chunk_alias_list[chunk_alias] = chunk_index;

    int x = chunk_index % MAP_CHUNK_WIDTH;
    int y = chunk_index / MAP_CHUNK_WIDTH;
    
    main_map->set_block(16*x,16*y,0, 1); //create chunk

/*
    This is evil, dont do this
*/
    struct MAP_CHUNK* m = main_map->chunk[chunk_index];

    memcpy( (char *) m->e, buff, byte_num);

}


//unsigned short chunk_alias;//
void clear_alias_StoC::handle()
{
    client_chunk_alias_list[chunk_alias] = -1;
}

//unsigned short chunk_alias;
//unsigned int chunk_index;
void set_map_alias_StoC::handle()
{
    client_chunk_alias_list[chunk_alias] = chunk_index;
    printf("Alias %i set to %i %i \n", chunk_alias, chunk_index%MAP_CHUNK_WIDTH, chunk_index /MAP_CHUNK_WIDTH);
}


void map_element_update::handle()
{
    printf("received map element update \n");
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

void map_chunk_compressed_StoC::handle(char* buff, int byte_num) {}
void map_chunk_uncompressed_StoC::handle(char* buff, int byte_size) {}

void clear_alias_StoC::handle() {}
void set_map_alias_StoC::handle() {}

void block_StoC::handle() {}
void map_metadata_StoC::handle() {}

#endif

}