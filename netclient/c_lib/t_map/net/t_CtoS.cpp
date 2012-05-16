#include "t_CtoS.hpp"

#include <t_map/t_map.hpp>

#include <state/server_state.hpp>
#include <item/toolbelt/_interface.hpp>
#include <item/_interface.hpp>
#include <item/container/container.hpp>
#include <item/container/_interface.hpp>
#include <item/item.hpp>
#include <item/config/item_attribute.hpp>

#include <agent/agent.hpp>
#include <agent/net_agent.hpp>

namespace t_map
{

#if DC_CLIENT
void block_CtoS::handle() {}
void create_container_block_CtoS::handle() {}
#endif

#if DC_SERVER
void block_CtoS::handle() 
{
    _set_broadcast(x,y,z, val);
}

void create_container_block_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.team == 0) return;

    Item::Item* placer = Item::get_item((ItemID)placer_id);
    if (placer == NULL) return;
    Item::ItemAttribute* attr = Item::get_item_attributes(placer->type);
    if (attr == NULL) return;
    int val = attr->placer_block_type_id;

    ItemContainerType container_type = get_container_type_for_block(val);
    if (container_type == CONTAINER_TYPE_NONE) return;

    // dont allow block to be set on existing block
    if (get(x,y,z) != 0) return;

    bool collides = false;
    _set(x,y,z, val); // set temporarily to test against
    if (agent_collides_terrain(a))
    {
        collides = true;
        //printf("collides w/ player\n");
    }
    else
    {
        for (int i=0; i<ServerState::agent_list->n_max; i++)
        {
            Agent_state* agent = ServerState::agent_list->a[i];
            if (agent == NULL || agent == a) continue;
            if (agent_collides_terrain(agent))
            {
                collides = true;
                break;
            }
        }
    }
    _set(x,y,z,0);  // unset

    if (collides) return;

    ItemContainer::ItemContainerInterface* container = ItemContainer::create_container(container_type);
    if (container == NULL) return;
    init_container(container);
    create_item_container_block(x,y,z, container->type, container->id);
    ItemContainer::send_container_create(client_id, container->id);

    Toolbelt::use_block_placer(a->id, (ItemID)placer_id);
    _set_broadcast(x,y,z, val);
    agent_placed_block_StoC msg;
    msg.id = a->id;
    msg.broadcast();
}

#endif

}   // t_map
