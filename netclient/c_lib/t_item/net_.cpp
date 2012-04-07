#include "net.hpp"

#include <c_lib/state/server_state.hpp>

#include <c_lib/t_item/interface.hpp>
//#include <c_lib/t_item/list.hpp>

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


inline void t_item_pick_up_CtoS::handle() {}

#endif

#if DC_SERVER
inline void t_item_picked_up_StoC::handle() {}
inline void t_item_create_StoC::handle() {}
inline void t_item_destroy_StoC::handle() {}


inline void t_item_pick_up_CtoS::handle()
{
    printf("agent %d attempt to pick up %d\n", client_id, id);
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("t_item_pick_up_CtoS :: agent %d not found\n", client_id);
        return;
    }
}

#endif
