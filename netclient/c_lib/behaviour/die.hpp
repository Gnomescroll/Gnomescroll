#pragma once

//forward declaration
#if DC_SERVER
namespace ServerState
{
void damage_objects_within_sphere(
    float x, float y, float z, float radius,
    int dmg, int owner,
    Object_types inflictor_type, int inflictor_id,
    bool suicidal//=true
);
    extern Agent_list* agent_list;
}
#endif

#if DC_CLIENT
namespace ClientState
{
    extern Agent_list* agent_list;
}
#include <c_lib/animations/animations.hpp>
#endif

/* die behaviours */

void dieExplode(ObjectState* state, Vec3 position, int owner)
{
    #if DC_SERVER
    ServerState::damage_objects_within_sphere(
        position.x, position.y, position.z,
        state->explosion_radius, state->explosion_damage, owner,
        state->type, state->id,
        state->suicidal
    );
    #endif
}

void dieTeamItemAnimation(Vec3 position, int team)
{
    #if DC_CLIENT
    Animations::team_item_explode(position, team);
    #endif
}

void dieRevokeOwner(Object_types type, int owner)
{
    if (owner != NO_AGENT)
    {
        Agent_state* a = STATE::agent_list->get(owner);
        if (a != NULL)
            a->status.lose_item(type);
    }
}
