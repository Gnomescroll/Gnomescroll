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

const int FREE_ITEM_MAX = 1024;
const int ITEM_TTL = 300; //5 minutes

class Free_item_list: public Object_list<Free_item, FREE_ITEM_MAX>
{
    private:
        const char* name() { return "Free_item"; }
    public:
        Free_item_list() {}


        void draw()
        {
            for (int i=0; i<n_max; i++)
            {
                if (a[i] == NULL) continue;
                a[i]->draw();
            }
        }

        void tick()
        {
            for (int i=0; i<n_max; i++)
            {
                if (a[i] == NULL) continue;
                a[i]->tick();
                if (++a[i]->ttl >= ITEM_TTL)destroy(a[i]->id);
            }
        }      
};
}