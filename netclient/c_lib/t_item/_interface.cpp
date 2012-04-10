#include "_interface.hpp"

//#include <c_lib/t_item/list.hpp>

#include <c_lib/t_item/free_item.hpp>
#include <c_lib/t_item/item_container.hpp>
#include <c_lib/t_item/item.hpp>

#include <c_lib/t_item/net/StoC.hpp>

namespace t_item
{

Free_item_list* free_item_list = NULL;
ItemContainerList* item_container_list = NULL;
ItemList* item_list = NULL;

void state_init()
{
    if (free_item_list != NULL)
    {
        printf("WARNING: attempt to call t_item::state_init() more than once\n");
        return;
    }
    free_item_list = new Free_item_list;
    item_container_list = new ItemContainerList;
    item_list = new ItemList;
}

void state_teardown()
{
    if (free_item_list != NULL) delete free_item_list;
    if (item_container_list != NULL) delete item_container_list;
    if (item_list != NULL) delete item_list;
}
 
void tick()
{
    free_item_list->tick();
}

void draw()
{
    free_item_list->draw();
}

void check_item_pickups()
{
    #ifdef DC_CLIENT
        printf("Warning: check_item_pickups was called on client! OOPS\n");
    #endif
    free_item_list->check_item_pickups();
}

void create_free_item(int item_type, 
    float x, float y, float z, 
    float vx, float vy, float vz)
{
    
    Free_item* f = free_item_list->create();
    if(f == NULL) return;
    f->set_state(x,y,z,vx,vy,vz);

    class free_item_create_StoC p;

    p.type = 0;
    p.id = f->id;
    p.x = x;
    p.y = y;
    p.z = z;
    p.mx = vx;
    p.my = vy;
    p.mz = vz;

    p.broadcast();
}

}
 
