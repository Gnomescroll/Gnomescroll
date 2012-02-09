#include "net_CtoS.hpp"

#include "net_StoC.hpp"
#include "t_map.hpp"

using namespace t_map;

#ifdef DC_SERVER
    
    void map_chunk_list_request_CtoS::handle() 
    {
        x = x*16 + 8;
        y = y*16 + 8;
                
        printf("map_chunk_list_request_CtoS: client= %i, x= %i y= %i r= %i \n", client_id, x,y,r);

        const int r2 = r*r;

        for(int i = 0; i < MAP_WIDTH/16; i++ )
        for(int j = 0; j < MAP_HEIGHT/16; j++)
        {
            class MAP_CHUNK* c = main_map->chunk[j*(MAP_WIDTH/16) + i];
            if( c == NULL ) continue;

            int p1 = (c->xpos+8) - x;
            int p2 = (c->ypos+8) - x;

            if( p1*p1 + p2*p2 > r2)
            {
                printf("sending chunk meta to client %i \n", client_id);
                class chunk_meta_data_StoC m;
                m.chunk_x = i;
                m.chunk_y = j;
                m.version = 1;
                m.sendToClient(client_id);
            }
        }
    }

    void block_CtoS::handle() 
    {
        _set_broadcast(x,y,z, val);
    }

#endif

#ifdef DC_CLIENT

void map_chunk_list_request_CtoS::handle() {}
void block_CtoS::handle() {}

#endif