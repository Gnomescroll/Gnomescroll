#include "components.hpp"

namespace Components
{

void init()
{
    init_interfaces();
    init_components();
}

void teardown()
{
    teardown_interfaces();
    teardown_components();
}

void init_config()
{
    init_components_config();
}

#if DC_SERVER
EntityID get_spawner_for_user(UserID user_id)
{
    auto a = GET_COMPONENT_LIST(AgentSpawner);
    for (size_t i=0, j=0; i<a->max && j<a->count; i++)
        if (a->components[i] != NULL)
        {
            j++;
            for (size_t j=0; j<a->components[i]->users.count; j++)
                if (a->components[i]->users.subscribers[j] == user_id)
                    return a->components[i]->entity->id;
        }
    return BASE_SPAWN_ID;
}

void revoke_owned_entities(AgentID owner_id)
{
    auto a = GET_COMPONENT_LIST(Owner);
    for (size_t i=0, j=0; i<a->max && j<a->count; i++)
    {
        OwnerComponent* owner = a->components[i];
        if (owner != NULL)
        {
            j++;
            if (owner->owner == owner_id)
                owner->revoke();
        }
    }
}
#endif

} // Components
