#pragma once

#include <t_map/_interface.hpp>
#include <t_map/t_properties.hpp>

namespace t_plant
{

class BlockList
{

	CubeType bl[16]; //block lookup

	enum
	{
		tree_error,
		tree_master,
		tree_root,
		tree_trunk_alive,
		tree_trunk_dead,
		tree_leave_alive,
		tree_leave_dead,
	} BlockType;

	enum
	{
		tree_alive,
		tree_dead,
	} BlockState;

	struct BlockArrayElement
	{
		int x,y,z;
		int block_id;
		int type;
		int state;
	};

	struct BlockArrayElement* ba;
	int ban;
	int bam;

	BlockList()
	{
		ban = 0;
		bam = 1024;
		ba = new struct BlockArrayElement[bam];

		for(int i=0;i<bam;i++)
			ba[i].type = tree_error;
	}

	~BlockList()
	{

	}

	void add(int x, int y, int z, int type, int state)
	{
		if(ban == bam)
		{
			printf("Warning: BlockList::add failed, list full \n");
			return;
		}

		ba[ban].x = x;
		ba[ban].y = y;
		ba[ban].z = z;
		ba[ban].type  = type;
		ba[ban].state = state;
	}

	void remove(int x, int y, int z)
	{

		for(int i=0;i<ban;i++)
		{
			if(ba[i].x == x && ba[i].y == y && ba[i].z == z)
			{
				GS_ASSERT(ban > 0);
				ban--;
				ba[i] = ba[ban];
				return;
			}
		}
		printf("ERROR BlockList: remove, could not remove block, block not in list \n");
	}

	void check_blocks()
	{
		for(int i=0; i<bam; i++)
		{
			// do something?
		}
	}
};


struct PlantExampleStruct
{
	int x,y,z;	//tree master
	m.alive;
};

void plant_example_init(struct PlantExampleStruct &m)
{
	//printf("plant_example_init: init plant \n");

	m.x = plant_placement_data.x;
	m.y = plant_placement_data.y;
	m.z = plant_placement_data.z;

	m.alive = true;

	printf("plant_example_init: %d %d %d \n", m.x,m.y,m.z);

	CubeType cube_id = t_map::get_cube_type("battery2");
	t_map::set(m.x,m.y,m.z, cube_id);
	t_map::broadcast_set_block(m.x,m.y,m.z, cube_id);

}

void plant_example_teardown(struct PlantExampleStruct &m)
{

}

void plant_example_tick(struct PlantExampleStruct &m)
{
	if(t_map::get(m.x,m.y,m.z) != t_map::get_cube_type("tree_example_master"))
	{
		m.alive = false;
	}

	for(int)
}


}