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

void relax_positions()
{   // pushes all PositionMomentum components away from each other
    const float relax_distance = 0.85f;
    const float relax_distance_sq = relax_distance * relax_distance;
    const float relax_force = 0.1f;

    auto list = GET_COMPONENT_LIST(PositionMomentum);
    IF_ASSERT(list == NULL) return;

    for (size_t i=0, j=0; i<list->max && j<list->count; i++)
    {
        auto physics = list->components[i];
        if (physics == NULL)
            continue;
        j++;
        const Vec3 position = physics->get_position();
        Vec3 momentum = physics->get_momentum();
        for (size_t m=0, n=0; m<list->max && n<list->count; m++)
        {
            auto other = list->components[m];
            if (other == NULL)
                continue;
            n++;
            if (other == physics)
                continue;

            Vec3 p = other->get_position();
            float dsq = vec3_distance_squared(position, p);
            if (dsq > relax_distance_sq)
                continue;
            Vec3 push = vec3_sub(position, p);
            if (dsq == 0)
                push = vec3_init(2*randf()-1, 2*randf()-1, 0.0f);
            push = vec3_normalize(push);
            push = vec3_scalar_mult(push, relax_force);
            momentum = vec3_add(momentum, push);
        }
        physics->set_momentum(momentum);
    }
}
#endif

} // Components
