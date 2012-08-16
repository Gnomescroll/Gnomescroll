#pragma once

#include <t_map/_interface.hpp>
#include <t_map/common/map_element.hpp>

namespace t_map
{

/*
	Update skylights seperately
*/

//e[ (z<<8)+((y&15)<<4)+(x&15) ] = e;
void update_skylight(int chunk_i, int chunk_j)
{


	class MAP_CHUNK* mc = main_map->chunk[32*chunk_j + chunk_i];
	struct MAP_ELEMENT e;

	//struct MAP_e e;

	for(int i=0; i<16; i++)
	for(int j=0; j<16; j++)
	{
		int k = 127;


		while(k > 0)
		{
			e = mc->get_element(i,j,k);
			if(e.block != 0)	//iterate until we hit top block
				break;

			e.light  |= 0xf0; //upper bits for skylight
			mc->set_element(i,j,k,e);
			k--;
		}

		for(int _k=0; _k<16; _k++)
		{
			k--;
			e = mc->get_element(i,j,k);
			if(e.block != 0)
				continue;
			e.light  |= 16*(15-_k); //clear upper bits
			mc->set_element(i,j,k,e);
		}

		while(k > 0)
		{
			k--;
			e = mc->get_element(i,j,k);
			if(e.block != 0)
				continue;
			e.light  |= 0x0f; //clear upper bits
			mc->set_element(i,j,k,e);
		}
	}

}


}