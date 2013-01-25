#include "_interface.hpp"

#include <t_plant/_data.hpp>

namespace t_plant
{


	void init()
	{
		init_data();
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

	}


}