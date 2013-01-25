#pragma once

namespace t_plant
{
/*
	class BlockList
	{

	};
*/
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

		void init()
		{
			block_list_n = 0;

		}

		void teardown()
		{

		}

		void tick()
		{

		}

	};


}