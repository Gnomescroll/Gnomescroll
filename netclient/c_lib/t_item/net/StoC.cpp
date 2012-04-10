#include "StoC.hpp"



#ifdef DC_CLIENT
#include <c_lib/t_item/_interface.hpp>

//#include <c_lib/t_item/client/_interface.hpp>
#endif

namespace t_item
{


#if DC_CLIENT

inline void free_item_picked_up_StoC::handle()
{
    //printf("%d picked up by %d\n", id, agent_id);
    // move item to inventory
}


inline void free_item_create_StoC::handle()
{
    //printf("creating %d\n", id);
    // create item via list
    t_item::free_item_list->create(id, x,y,z,mx,my,mz);
}

inline void free_item_destroy_StoC::handle()
{
    //printf("destroying %d\n", id);
    // destroy item via list
    t_item::free_item_list->destroy(id);
}



inline void assign_agent_inventory_StoC::handle()
{
	t_item::player_inventory_id = inventory_id;
	printf("Player %i assigned inventory id %i \n", agent_id, inventory_id);



}


#endif

#if DC_SERVER

inline void free_item_picked_up_StoC::handle() {}
inline void free_item_create_StoC::handle() {}
inline void free_item_destroy_StoC::handle() {}

inline void assign_agent_inventory_StoC::handle() {}
#endif

}
