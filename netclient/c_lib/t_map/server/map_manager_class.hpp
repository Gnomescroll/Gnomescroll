#pragma once

#include <c_lib/t_map/t_map_class.hpp>
#include <c_lib/t_map/t_map.hpp>

#include <c_lib/t_map/net/t_StoC.hpp>

namespace t_map
{

const int  DEFAULT_MAP_MANAGER_RADIUS = 3;
const int UNSUB_DISTANCE = 5;
const int UNSUB_DISTANCE2 = UNSUB_DISTANCE*UNSUB_DISTANCE;

const int MAP_MANAGER_ALIAS_LIST_SIZE = 512;

//alias constants
const unsigned short NO_ALIAS = 0xffff;

const unsigned short SUBSCRIBED = 0;
const unsigned UNSUBSCRIBED_NO_DATA = 0xffff;

/*
    state:
    non-subscribed, no data
    non-subscribed, version number
    subscribed

    0xffff
    0x0000
*/
class MAP_MANAGER_ELEMENT
{
    public:
    unsigned short version;

    MAP_MANAGER_ELEMENT()
    {
        version = UNSUBSCRIBED_NO_DATA;    
    }
};

class Map_manager
{
    public:
    class Terrain_map* t;

    int client_id;

    bool needs_update;
    int xpos; //player chunk position
    int ypos;
    int radius;

    int xchunk_dim;
    int ychunk_dim;
    
    struct MAP_MANAGER_ELEMENT* version_list;
    
    int subed_chunks;
    int alias_list[ MAP_MANAGER_ALIAS_LIST_SIZE ];  //aliases are ints

    Map_manager(int _client_id)
    {
        needs_update = false;
        client_id = _client_id;
        
        radius = DEFAULT_MAP_MANAGER_RADIUS;

        subed_chunks = 0;
        xpos = 0xffff;
        ypos = 0xffff;

        t = get_map();
        xchunk_dim = t->xchunk_dim;
        ychunk_dim = t->ychunk_dim;

        version_list = new MAP_MANAGER_ELEMENT[xchunk_dim*ychunk_dim];
        for(int i=0; i < MAP_MANAGER_ALIAS_LIST_SIZE; i++) alias_list[i] = NO_ALIAS;
    }

    ~Map_manager()
    {
        delete[] version_list;
    }

    void update();
    void set_position(int x, int y);

    private:

    //int distance2(int alias) __attribute((always_inline));
    //int get_farthest_subbed_chunk() __attribute((always_inline));

    void sub(int x, int y);
    void unsub(int alias);

    void send_alias(int alias, int index) __attribute((always_inline))
    {
        set_map_alias_StoC c;
        c.chunk_alias = alias;
        c.chunk_index = index;
        c.sendToClient(client_id);
    }

    void send_compressed_chunk(int alias, int index);

    void send_delta() {}
};

void Map_manager::update()
{
    if(needs_update == false) return;

    int imin = (xpos - radius) > 0 ? xpos - radius : 0;
    int jmin = (ypos - radius) > 0 ? ypos - radius : 0;

    int imax = (xpos+radius) < xchunk_dim ? xpos+radius : xchunk_dim;
    int jmax = (ypos+radius) < ychunk_dim ? ypos+radius : ychunk_dim;

    //printf("xpos,ypos= %i %i imin,jmin= %i %i imax,jmax= %i %i \n", xpos,ypos, imin,jmin, imax,jmax);

/*

    Crude method: 
*/

    //printf("unsub\n");

    for(int i=0; i< MAP_MANAGER_ALIAS_LIST_SIZE; i++)
    {
        if( alias_list[i] == NO_ALIAS ) continue;
        int x = (alias_list[i] % xchunk_dim);
        int y = (alias_list[i] / xchunk_dim);

        //printf("alias= %i index=%i \n", i, alias_list[i] );

        x = x - xpos;
        y = y - ypos; 



        if( x*x + y*y > UNSUB_DISTANCE2 ) unsub(i);
    }

    //printf("sub\n");
    for(int i=imin;i<imax; i++)
    for(int j=jmin;j<jmax; j++)
    {
        if( (xpos - i)*(xpos - i) + (ypos - j)*(ypos - j) >= radius ) continue;
        if( version_list[j*xchunk_dim + i].version == SUBSCRIBED ) continue;
        //printf("sub %i %i \n", i,j);
        sub(i,j);
    }

}

void Map_manager::send_compressed_chunk(int alias, int index)
{

    //printf("sending compressed chunk! \n");

    //unsigned short chunk_alias;
    //int chunk_index;
    //int byte_size;

    if(t->chunk[index] == NULL)
    {
        printf("Chunk is null!  Handle this! \n");
        return;
    }
    map_chunk_uncompressed_StoC c;

    c.chunk_alias = alias;
    c.chunk_index = index;
    c.byte_size = 4*16*16*128;

    int size = sizeof(struct MAP_ELEMENT)*16*16*TERRAIN_MAP_HEIGHT;
    c.byte_size = size;
    c.sendToClient(client_id, (char*)t->chunk[index]->e, size);

    //void sendToClient(int client_id, char* buff, int len) 
    //map_chunk_uncompressed_StoC
}

//this is chunk position!
void Map_manager::set_position(int x, int y)
{
    if(x != xpos || y != ypos) needs_update = true;
    xpos = x;
    ypos = y;      
}

void Map_manager::sub(int x, int y)
{
    printf("sub: %i %i \n", x, y);

    int index = y*xchunk_dim + x;

    //int version = version_list[index].version;
    if( subed_chunks == MAP_MANAGER_ALIAS_LIST_SIZE)
    {
        printf("FIX THIS!!! Map_manager::sub, alias list maxed out \n");
        return;
    }

    int alias=0;
    while( alias_list[alias] != NO_ALIAS) alias++;
    //set alias
    alias_list[alias] = index;

    //printf("alisas = %i index = %i \n", alias, index);
    
    //grab chunk
    map_history->chunk[index].add_subscriber(client_id, alias, version_list[index].version);

    version_list[index].version = SUBSCRIBED;   //set subscription property
    subed_chunks++;

    send_alias(alias, index);
    //send alias to client
    send_compressed_chunk(alias, index);

    //send alias to client
    /*
        send alias to client
        compress map chunk

    */
}

/*
int Map_manager::distance2(int alias)
{
    int index = alias_list[alias];

    int x = index % x_chunk_size;
    int y = index / x_chunk_size;

    x = x - xpos;
    y = y - ypos;

    return x*x+y*y;
}

int Map_manager::get_farthest_subbed_chunk()
{
    //get first non-zero alias
    int alias = 0;
    while( alias_list[alias] == NO_ALIAS ) alias++;
    int distance = alias_list[alias]
    int lowest = alias;

    int index = alias_list[0]; 
}
*/

void Map_manager::unsub(int alias)
{

    int index = alias_list[alias];

    printf("unsub: %i %i \n", index % xchunk_dim, index / xchunk_dim);

    version_list[index].version = map_history->chunk[index].version; //cache version

    map_history->chunk[index].remove_subscriber(client_id);

    alias_list[alias] = NO_ALIAS;
    subed_chunks--;


    //send unsubscribed
}


}