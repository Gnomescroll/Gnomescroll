#include "StoC.hpp"



#ifdef DC_CLIENT
//#include <c_lib/state/server_state.hpp>
//#include <c_lib/t_item/free_item.hpp>
#include <c_lib/t_item/interface.hpp>
#endif

namespace t_item
{


#if DC_CLIENT

inline void t_item_picked_up_StoC::handle()
{
    printf("%d picked up by %d\n", id, agent_id);
    // move item to inventory
}


inline void t_item_create_StoC::handle()
{
    printf("creating %d\n", id);
    // create item via list
    t_item::free_item_list->create(id, x,y,z,mx,my,mz);
}

inline void t_item_destroy_StoC::handle()
{
    printf("destroying %d\n", id);
    // destroy item via list
    t_item::free_item_list->destroy(id);
}

#endif

#if DC_SERVER

inline void t_item_picked_up_StoC::handle() {}
inline void t_item_create_StoC::handle() {}
inline void t_item_destroy_StoC::handle() {}

#endif

}
