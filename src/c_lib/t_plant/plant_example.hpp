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
	int x,y,z;
};

void plant_example_init(struct PlantExampleStruct &m)
{
	printf("plant_example_init: init plant \n");

	m.x = 5;
	m.y = 5;
	m.z = 8;
}

void plant_example_teardown(struct PlantExampleStruct &m)
{

}

void plant_example_tick(struct PlantExampleStruct &m)
{

}


}