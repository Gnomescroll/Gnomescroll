#pragma once


/*
    List of Subscribers
    History of changes
    Compression
*/


/*

    Compress chunk and send
    Store history
    Store last compression
    Send last compression and then history

*/

//#include <c_lib/t_map/t_map_class.hpp>
//#include <c_lib/t_map/net/t_StoC.hpp>
#include <c_lib/t_map/common/map_element.hpp>

namespace t_map
{

/*
    unsigned char block;
    unsigned char palette;
    unsigned char damage;
    unsigned char lighting;
*/

struct CHUNK_HISTORY_ELEMENT
{
    unsigned short pos;
    struct MAP_ELEMENT e;
};

const int SMC_MAX_SUBSCRIBERS = 64;
const int SMC_HISTORY_ARRAY_SIZE = 256;

class MAP_CHUNK_HISTORY
{
    public:

    int version;

    unsigned short subscribers[ SMC_MAX_SUBSCRIBERS ];
    unsigned short chunk_aliases[ SMC_MAX_SUBSCRIBERS ]; 
    
    int subscriber_num;

    struct CHUNK_HISTORY_ELEMENT history_array[ SMC_HISTORY_ARRAY_SIZE ];
    int last_history;

    MAP_CHUNK_HISTORY()
    {
        version = 1;    //chunk version number

        subscriber_num = 0;
        memset( &history_array, 0, SMC_HISTORY_ARRAY_SIZE*sizeof(struct CHUNK_HISTORY_ELEMENT));
    }

    ~MAP_CHUNK_HISTORY()
    {
                
    }

    void add_subscriber(int client_id, int chunk_alias, int client_map_version);
    void remove_subscriber(int client_id);

    void block_change(int x, int y, int z, struct MAP_ELEMENT e)
    {
        
        for(int i=0; i < subscriber_num; i++)
        {
            //send element
        }
    }
};

class Terrain_map_history
{
    public:
    
    int xdim;
    int ydim;
    
    int xchunk_dim;
    int ychunk_dim;
    
    struct MAP_CHUNK_HISTORY* chunk;

    Terrain_map_history(int _xdim, int _ydim)
    {
        xdim = (_xdim/16)*16; 
        ydim = (_ydim/16)*16;
        xchunk_dim = _xdim/16; 
        ychunk_dim = _ydim/16;

        chunk = new MAP_CHUNK_HISTORY[xchunk_dim*ychunk_dim];
    }

    ~Terrain_map_history()
    {
        delete[] chunk;
    }

};

}