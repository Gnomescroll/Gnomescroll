#pragma once

#include <t_gen/twister.hpp>
#include <t_map/t_map.hpp>


namespace t_gen 
{

void add_terrain_features()
{
    printf("Adding terrain features\n");

	for (int i = 0; i < 300; i++) {
		int x = randrange(0, XMAX);
		int y = randrange(0, YMAX);
		int z = ZMAX;

		while (t_map::get(x, y, z) == 0) {
			z--;
		}

		z++; z++;

		for (int j = 0; j < 10; j++) {
			t_map::set(x, y, z + j, 1);
		}
	}
}

}   // t_gen
