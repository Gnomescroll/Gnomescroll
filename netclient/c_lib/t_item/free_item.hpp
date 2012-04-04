#pragma once


namespace t_item
{

class Free_item
{
	public:
	int id;
	int item_id;

	int ttl;
	int ttl_max;

	Free_item()
	{

	}

	static void init()
	{

	}

	static void teardown()
	{

	}
	
	void draw();
	void tick();
};

void Free_item::draw()
{

}

void Free_item::tick()
{

}


}