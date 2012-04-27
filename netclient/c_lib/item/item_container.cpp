#include "item_container.hpp"

#include <item/net/StoC.hpp>

namespace Item
{

void init_container(ItemContainer* container, ItemContainerType type)
{
    switch (type)
    {
        case AGENT_INVENTORY:
            container->init(AGENT_INVENTORY, AGENT_INVENTORY_X, AGENT_INVENTORY_Y);
            break;
        case AGENT_TOOLBELT:
            container->init(AGENT_TOOLBELT, AGENT_TOOLBELT_X, AGENT_TOOLBELT_Y);
            break;
        case AGENT_NANITE:
            container->init(AGENT_NANITE, AGENT_NANITE_X, AGENT_NANITE_Y);
            break;
        default:
            printf("init_container() - Unhandled container type %d\n", type);
            assert(false);
            break;
    }
}

/* Network */
#if DC_SERVER

#include <item/net/StoC.hpp>

//  tell client to assign container to an agent
void send_container_assign(class ItemContainer* container, int client_id)
{
    class assign_item_container_StoC p;
    p.container_id = container->id;
    p.container_type = container->type;
    p.agent_id = client_id;
    p.sendToClient(client_id);
}

void send_container_create(class ItemContainer* container, int client_id)
{
    class create_item_container_StoC p;
    p.container_id = container->id;
    p.container_type = container->type;
    p.agent_id = client_id;
    p.sendToClient(client_id);
}

void send_container_delete(class ItemContainer* container, int client_id)
{
    class delete_item_container_StoC p;
    p.container_id = container->id;
    p.container_type = container->type;
    p.agent_id = client_id;
    p.sendToClient(client_id);
}

#endif



}
