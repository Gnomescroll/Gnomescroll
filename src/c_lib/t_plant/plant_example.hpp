#pragma once

namespace t_plant
{
/*
	class BlockList
	{

	};
*/

struct PlantExampleStruct
{

};

void plant_example_init()
{

}

void plant_example_teardown()
{

}

void plant_example_tick()
{

}


class EXAMPLE_PLANT
{

	struct BLOCK_LIST_ELEMENT
	{
		int x,y,z;
		int state;

	};
	static const int BLOCK_LIST_MAX = 1024;
	struct BLOCK_LIST_ELEMENT block_list_array[BLOCK_LIST_MAX];
	int block_list_n;


};


}