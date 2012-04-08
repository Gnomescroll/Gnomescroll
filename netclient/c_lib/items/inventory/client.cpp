#include "client.hpp"

#if DC_CLIENT


void ClientInventory::attach_to_owner()
{
    Agent_state* a = STATE::agent_list->get(this->get_owner());
    if (a == NULL)
    {
        printf("WARNING: Inventroy::attach_to_owner() -- agent %d not found\n", this->get_owner());
        return;
    }
    if (a->status.inventory != NULL)
        printf("WARNING: reassigned agent inventory\n");
    a->status.inventory = this;
}

#endif
