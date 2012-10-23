#pragma once

#include <t_map/t_map.hpp>
#include <t_map/common/constants.hpp>

#include <t_gen/_interface.hpp>

namespace t_map
{

void regolith_post_processing()
{
    CubeID regolith_id = get_cube_id("regolith");

    class MAP_CHUNK* c;
        for(int i=0; i < MAP_CHUNK_XDIM*MAP_CHUNK_YDIM; i++)
        {
            c = main_map->chunk[i];
            if(c == NULL) continue;
            
            for(int z=0; z<TERRAIN_MAP_HEIGHT-1; z++)
            {
                for(int x=0; x<16; x++)
                for(int y=0; y<16; y++)
                {
                    struct MAP_ELEMENT e1;
                    struct MAP_ELEMENT e2;

                    e1 = c->get_element(x,y,z);
                    if(e1.block == regolith_id)
                    {
                        e2  = c->get_element(x,y,z+1);

                        if( isOccludes((CubeID)e2.block) )
                        {
                            e1.palette = 0;
                        }
                        else
                        {
                            e1.palette = 1;
                        }

                        c->set_element(x,y,z, e1);
                    }
                }
            }
        }
    //main_map->chunk[]

}


void map_post_processing()
{
    regolith_post_processing();
}

#if __GNUC__
    #if defined(__i386__)

    static __inline__ unsigned long long rdtsc(void)
    {
        unsigned long long int x;
        __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
        return x;
    }

    #elif defined(__x86_64__)

    static __inline__ unsigned long long rdtsc(void)
    {
        unsigned hi, lo;
        __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
        return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
    }

    #endif
#endif



#if DC_SERVER
void init_env_process();
void teardown_env_process();
void environment_process_tick();
void environment_process_startup();
#endif


}
