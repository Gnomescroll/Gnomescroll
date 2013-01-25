#include "_interface.hpp"

#include <t_plant/_data.hpp>
#include <t_plant/example_plant.hpp>

namespace t_plant
{


	void init()
	{
		init_data();

		register_plant(0, "example_plant", &plant_example_init, &plant_example_teardown, &plant_example_tick );
	}

	void teardown()
	{
		teardown_data();
	}

	void tick()
	{
		for(int i=0; i<plant_array.index; i++)
		{

		}

	}



	void create_plant(int x, int y, int z, int type)
	{
		GS_ASSERT(type == 0);


	}


}