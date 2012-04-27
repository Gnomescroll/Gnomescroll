#include "item_container.hpp"



namespace item
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
            printf("Unhandled container type %d\n", type);
            assert(false);
            break;
    }
}

}
