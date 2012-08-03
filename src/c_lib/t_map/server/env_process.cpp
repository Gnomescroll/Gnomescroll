#include "env_process.hpp"

namespace t_map
{

#if DC_SERVER

static const int RAS = 571; //random array size
int* _random = NULL;

void init_env_process()
{
    GS_ASSERT(_random == NULL);
    _random = (int*) malloc(RAS*sizeof(int));
    for (int i=0; i<RAS; i++) _random[i] = rand();
}

void teardown_env_process()
{
    if (_random != NULL) free(_random);
}

__attribute__((optimize("-O3")))
void environment_process_tick()
{
    GS_ASSERT(_random != NULL);
    if (_random == NULL) return;
    
    static int regolith_id = dat_get_cube_id("regolith");

    static int _random_index = rand();
    _random_index = (_random_index+rand() ) % RAS;

    static int x=0; 
    static int y=0;
    static int z=0;

    for(int i=0; i< 10000; i++)
    {
        struct MAP_ELEMENT e1 = get_element(x,y,z);

        if(e1.block == regolith_id)
        {
            //flip pallete if there is empty space above regolith
            if(e1.palette == 0)
            {
                struct MAP_ELEMENT e2 = get_element(x,y,z+1);
                
                if(isOccludes(e2.block) == 0)
                {
                    _random_index = (_random_index+1) % RAS;
                    if(_random[_random_index] % 7 == 0) //14% chance
                    {
                        broadcast_set_block_palette(x,y,z, e1.block, 1); //setting regolith
                    }
                }
            }
            //flip pallete if there is no space above regolith
            else if(e1.palette == 1)
            {
                struct MAP_ELEMENT e2 = get_element(x,y,z+1);
                
                if(isOccludes(e2.block) == 1)
                {
                    _random_index = (_random_index+1) % RAS;
                    if(_random[_random_index] % 3 == 0) //33% chance
                    {
                        broadcast_set_block_palette(x,y,z, e1.block, 0); //setting regolith
                    }
                }
            }

        }

        x++;
        if(x >= 512)
        {
            x=0;
            y++;
            if(y >= 512) 
            {
                y=0;
                z++;
                if(z >= 128)
                {
                    z = 0;
                }
            }
        }
    }
}
#endif



}   // t_map
