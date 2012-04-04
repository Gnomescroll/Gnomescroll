#pragma once

#include <c_lib/t_item/free_item.hpp>

namespace t_item
{

#define FREE_ITEM_MAX 1024

class Free_item_list: public Object_list<Free_item, FREE_ITEM_MAX>
{
    private:
        const char* name() { return "Free_item"; }

    public:

        Free_item_list();


        void draw();
        void tick();

};

void Free_item_list::draw()
{
    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        a[i]->draw();
    }
}

void Free_item_list::tick()
{
    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->ttl >= a[i]->ttl_max)
            destroy(a[i]->id);
    }
}
	
	void init()
	{


	}

	void tear_down()
	{


	}

	void tick()
	{

	}

	void draw()
	{

	}

}