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

template <class Object>
void dieExplode(ObjectState* state, Object* object)
{
    #if DC_SERVER
    Vec3 p = state->get_position();
    ServerState::damage_objects_within_sphere(
        p.x, p.y, p.z,
        state->explosion_radius, state->explosion_damage, object->get_owner(),
        state->type, state->id,
        state->suicidal
    );
    #endif
}

template <class Object>
void dieBroadcast(ObjectState* state, Object* object)
{
    #if DC_SERVER
    object_destroy_StoC msg;
    msg.id = state->id;
    msg.type = state->type;
    msg.broadcast();
    #endif
}

void dieTeamItemAnimation(Vec3 position, int team)
{
    #if DC_CLIENT
    Animations::team_item_explode(position, team);
    #endif
}

template <class Object>
void dieRevokeOwner(ObjectState* state, Object* object)
{
    int owner = object->get_owner();
    if (owner != NO_AGENT)
    {
        Agent_state* a = STATE::agent_list->get(owner);
        if (a != NULL)
            a->status.lose_item(state->type);
    }
}
