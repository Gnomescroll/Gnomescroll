#pragma once

#include <t_map/t_map.hpp>
#include <t_map/common/constants.hpp>

#include <t_gen/_interface.hpp>

namespace t_map
{

void regolith_post_processing()
{
    int regolith_id = dat_get_cube_id("regolith");

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

	            		if( isOccludes(e2.block) )
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
__attribute__((optimize("-O3")))
void environment_process_tick()
{
	static int init = 0;
	static int* _random;
	static int _random_index = 0;

	static int regolith_id;
	if(init == 0)
	{
		_random = new int[256];
		for(int i=0; i<256; i++) _random[i] = rand();
		
		regolith_id = dat_get_cube_id("regolith");
		init = 1;
	}

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
		    		_random_index = (_random_index+1) % 256;
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
		    		_random_index = (_random_index+1) % 256;
		    		if(_random[_random_index] % 3 == 0)	//33% chance
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

		//if(x==0 && y==0 && z==0) printf("loop: \n");
	}



}
#endif

}
