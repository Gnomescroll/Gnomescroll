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


void rock_layer_post_processing()
{
	float* _2d_noise_array = new float [512*512];
	int* regolith_depth_array = new int[512*512]; 

	const int seed = 58412;
	const float persistance = 0.85f;
	const float octaves = 4.0f;

	t_gen::populate_2d_noise_array(_2d_noise_array, seed, persistance, octaves);

	for(int i=0; i<512; i++)
	for(int j=0; j<512; j++)
	{
		regolith_depth_array[512*j+i] = 1.0f + 3.0f*abs(_2d_noise_array[ 512*j+i]);

		//printf("depth= %i \n", regolith_depth_array[512*j+i] );
	}

    int regolith = dat_get_cube_id("regolith");
    int rock_layer = dat_get_cube_id("rock_layer");
    rock_layer = regolith;

    const static int regolith_depth = 12;

    class MAP_CHUNK* c;
        for(int i=0; i < MAP_CHUNK_XDIM*MAP_CHUNK_YDIM; i++)
        {
        	//int _i = i % 32;
        	//int _j = i / 32;

            c = main_map->chunk[i];
            if(c == NULL) continue;
            
            for(int x=0; x<16; x++)
            for(int y=0; y<16; y++)
            {
	       		//int depth = regolith_depth_array[ 512*(_j+y) + (_i+x)];

	            int run = 0;
	            for(int z=TERRAIN_MAP_HEIGHT-1; z>0; z--)
	            {

		            	struct MAP_ELEMENT e1;
		            	//struct MAP_ELEMENT e2;

		            	e1 = c->get_element(x,y,z);

		            	if(e1.block != 0)
		            	{
		            		run++;
		            	}
		            	else
		            	{
		            		run = 0;
		            	}


		            	if(run > regolith_depth && e1.block == regolith)
		            	//if(run > depth && e1.block == regolith)
		            	{
		            		e1.block = rock_layer;
		            		c->set_element(x,y,z, e1);
		            	}
	        	}
        	}
        }
    //main_map->chunk[]
    delete[] regolith_depth_array;
    delete[] _2d_noise_array;
}


void map_post_processing()
{
    regolith_post_processing();
    rock_layer_post_processing();
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
void environment_process_tick()
{
	static int run = 0;
	static int _random[256];
	static int _random_index = 0;
	if(run % 256 == 0)
	{
		for(int i=0; i<256; i++) _random[i] = rand();
		run = 0;
	}
	run++;


	static int x=0; 
	static int y=0;
	static int z=0;

	const static int regolith_id = dat_get_cube_id("regolith");

	for(int i=0; i< 200000; i++)
	{


    	struct MAP_ELEMENT e1 = get_element(x,y,z);
    	struct MAP_ELEMENT e2 = get_element(x,y,z+1);

    	//if(e1.block == regolith_id && e2.block == 0)
    	
    	if(e1.block == regolith_id && e1.palette == 0 && isOccludes(e2.block) == 0)
    	{
    		broadcast_set_block_palette(x,y,z, e1.block, 1); //setting regolith
    		//printf(": %i %i %i \n",x,y,z);
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

		if(x==0 && y==0 && z==0) printf("loop: \n");
	}



}
#endif

}
