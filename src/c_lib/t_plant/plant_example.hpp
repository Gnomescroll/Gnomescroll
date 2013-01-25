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
	//printf("x,y,z= %d %d %d \n", m.x,m.y,m.z);
}


/*
	//registering callbacks

	register_plant(0, "plant_example", sizeof(struct PlantExampleStruct));
	register_plant_function("plant_example", "init",		(fptr_void) &plant_example_init);
	register_plant_function("plant_example", "teardown",	(fptr_void) &plant_example_teardown);
	register_plant_function("plant_example", "tick",		(fptr_void) &plant_example_tick );
*/

}