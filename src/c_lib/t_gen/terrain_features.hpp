#pragma once

#include <t_gen/twister.hpp>
#include <t_map/t_map.hpp>

#include <physics/vec3.hpp>



namespace t_gen {
	const float DOUBLE_PI = PI * 2;
	const float HALF_PI = PI / 2;
	const float QUARTER_PI = HALF_PI / 2;
	const float EIGHTH_PI = QUARTER_PI / 2;

	void make_circle(int x, int y, int z, float dist, int block) {
		float fx = 0;
		float fy = 0;
		float angle = 0;

		while (angle < DOUBLE_PI) {
			fx = sinf(angle) * dist;
			fy = cosf(angle) * dist;
			t_map::set(x + (int)fx, y + (int)fy, z, block);
			angle += EIGHTH_PI / 8;
		}
	}

	void add_terrain_features() {
		printf("Adding terrain features\n");

		for (int trees = 0; trees < 200; trees++) {
			int x, y, z;
			
			// find appropriate ground for trunk
			do {
				x = randrange(0, XMAX);
				y = randrange(0, YMAX);
				z = ZMAX;

				while (t_map::get(x, y, z) == 0) {
					z--;
				}
			} while (t_map::get(x, y, z) != t_map::get_cube_id("regolith") );

			// set bark/leaf textures
			int leaves = t_map::get_cube_id("leaves");
			if (randrange(0, 1) == 0) 
				leaves = t_map::get_cube_id("leaves_red");

			int trunk = t_map::get_cube_id("space_tree_trunk");
			if (randrange(0, 1) == 0) 
				trunk = t_map::get_cube_id("space_tree_trunk2");

			int segs = randrange(2, 9);
			for (int seg = 0; seg < segs; seg++) {
				// height of current trunk segment
				int height = randrange(6, 12);
				if (seg == 0) 
					height = randrange(12, 21); // ensure trunk goes up aways

				for (int j = 0; j < height; j++) {
					t_map::set(x, y, z + j, trunk);

					if (j == height - 1) {
						float dist = randrange(2, 11);
						for (int i = -dist; i < dist; i++)	t_map::set(x+i, y, z+j, trunk); // limbs
						for (int i = -dist; i < dist; i++)	t_map::set(x, y+i, z+j, trunk); // limbs
						while (dist > 0) {
							make_circle(x, y, z+j, dist, leaves);
							make_circle(x, y, z+j+1, dist+1, leaves);
							make_circle(x, y, z+j+2, dist+.5, leaves);
							dist -= 2.5;
						}
					}
				}

				z += height;
			}
		}
	}
}   // t_gen
