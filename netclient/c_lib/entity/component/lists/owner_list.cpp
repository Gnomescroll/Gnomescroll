#include "owner_list.hpp"

#include <c_lib/agent/constants.hpp>
#include <c_lib/entity/components/owner.hpp>

namespace Components
{

void OwnerComponentList::transfer(int owner_id, int new_owner_id)
{
    OwnerComponent* owner;
    for (int i=0; i<this->max; i++)
    {
        owner = (OwnerComponent*)this->components[i];
        if (owner == NULL) continue;
        if (owner->owner == owner_id) owner->owner = new_owner_id;
    }
}

#if DC_SERVER
void OwnerComponentList::revoke(int owner_id)
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
