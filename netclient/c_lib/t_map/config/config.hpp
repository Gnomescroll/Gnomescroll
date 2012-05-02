#pragma once

#include "config_util.hpp"
#include "drop_table.hpp"

namespace t_map
{

void load_block_dat()
{

 	int test = 10;
 	printf("test= %i \n", test);

	int t00 = texture_alias("media/sprites/i00.png");

	cube_def(0, 0 , "TEST_RESOURCE1");
	sprite_def(t00, 0,0);

	cube_def(1, 0 , "TEST_RESOURCE2");
	sprite_def(t00, 1,0);

	cube_def(2, 0 , "TEST_RESOURCE3");
	sprite_def(t00, 0,2);

}

void load_map_drop_dat()
{

	def_drop("regolith");
	
	end_drop_dat();
}


}
