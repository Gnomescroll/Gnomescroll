#pragma once

namespace t_item
{

#define FREE_ITEM_MAX 1024

class Free_item_list: public Object_list<Free_item_list, FREE_ITEM_MAX>
{
    private:
        const char* name() { return "Free_item"; }

    public:

        Free_item_list();


        void draw()
        {
        	//enable and bind stuff
		    for (int i=0; i<n_max; i++)
		    {
		        if (a[i] == NULL) continue;
		        a[i]->draw();
		    }
		    //enable and bind stuff
        }

        void tick()
        {
		    for (int i=0; i<n_max; i++)
		    {
		        if (a[i] == NULL) continue;
		        a[i]->tick();
		        if (a[i]->ttl >= a[i]->ttl_max)
		            destroy(a[i]->id);
		    }
        }

};

	
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