#include "owner_list.hpp"

#include <agent/constants.hpp>
#include <entity/components/owner.hpp>

namespace Components
{

void OwnerComponentList::transfer(AgentID owner_id, AgentID new_owner_id)
{
    for (int i=0; i<this->max; i++)
    {
        OwnerComponent* owner = (OwnerComponent*)this->components[i];
        if (owner == NULL) continue;
        if (owner->owner == owner_id)
            owner->set_owner(new_owner_id);
    }
}

#if DC_SERVER
void OwnerComponentList::revoke(AgentID owner_id)
{
    OwnerComponent* owner;
    for (int i=0; i<this->max; i++)
    {
        owner = (OwnerComponent*)this->components[i];
        if (owner == NULL) continue;
        if (owner->owner == owner_id) owner->revoke();
    }
}
#endif

} // Components
