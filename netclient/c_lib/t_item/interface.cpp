#include "interface.hpp"

//#include <c_lib/t_item/list.hpp>

#include <c_lib/t_item/free_item.hpp>
#include <c_lib/t_item/net/StoC.hpp>

namespace t_item
{

Free_item_list* free_item_list = NULL;

void state_init()
{
    if (free_item_list != NULL)
    {
        printf("WARNING: attempt to call t_item::state_init() more than once\n");
        return;
    }
    free_item_list = new Free_item_list;
}

void state_teardown()
{
    if (free_item_list != NULL)
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



void create_free_item(int id, float x, float y, float z, float vx, float vy, float vz)
{
    
    Free_item* f = free_item_list->create();
    if(f == NULL) return;
    f->set_state(x,y,z, vx,vy,vz);

    class free_item_create_StoC p;





}

}
 