#include "CtoS.hpp"

#if DC_SERVER
#include <state/server_state.hpp>
#include <t_item/_interface.hpp>
#endif

namespace t_map
{


#if DC_SERVER

/*
void t_item_pick_up_CtoS::handle()
{
    printf("agent %d attempt to pick up %d\n", client_id, id);
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL)
    {
        printf("t_item_pick_up_CtoS :: agent %d not found\n", client_id);
        return;
    }
}
*/
#endif



#if DC_CLIENT
/*
void t_item_pick_up_CtoS::handle() {}
*/
#endif


}