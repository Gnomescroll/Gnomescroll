#include "interface.hpp"

#include "list.hpp"

namespace t_item
{

Free_item_list* free_item_list = NULL;

void state_init()
{
	free_item_list = new Free_item_list;
}

void state_teardown()
{
	delete free_item_list;
}

void tick()
{
	free_item_list->tick();
}

void draw()
{
	free_item_list->draw();
}

}