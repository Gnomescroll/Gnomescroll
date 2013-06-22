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

#if DC_SERVER
EntityID get_spawner_for_user(UserID user_id)
{
    AgentSpawnerComponentList* a = agent_spawner_component_list;
    for (int i=0; i<a->max; i++)
        if (a->components[i] != NULL)
            for (size_t j=0; j<a->components[i]->users.count; j++)
                if (a->components[i]->users.subscribers[j] == user_id)
                    return a->components[i]->entity->id;
    return BASE_SPAWN_ID;
}

void revoke_owned_entities(AgentID owner_id)
{
    OwnerComponent* owner;
    for (int i=0; i<owner_component_list->max; i++)
    {
        owner = (OwnerComponent*)owner_component_list->components[i];
        if (owner != NULL &&owner->owner == owner_id)
            owner->revoke();
    }
}
#endif

} // Components
