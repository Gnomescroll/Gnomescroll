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

template <class Super, class Object>
class DieExplode: public Super
{
    protected:
    inline void die(ObjectState* state, Object* object)
    {
        #if DC_SERVER
        Vec3 p = state->get_position();
        ServerState::damage_objects_within_sphere(
            p.x, p.y, p.z,
            state->explosion_radius, state->explosion_damage, state->owner,
            state->type, state->id,
            state->suicidal
        );
        #endif
        Super::die(state, object);
    }
};

template <class Super, class Object>
class DieBroadcast: public Super
{
    protected:
    inline void die(ObjectState* state, Object* object)
    {
        #if DC_SERVER
        object_destroy_StoC msg;
        msg.id = state->id;
        msg.type = state->type;
        msg.broadcast();
        #endif
        Super::die(state, object);
    }
};

template <class Super, class Object>
class DieTeamItemAnimation: public Super
{
    protected:
    inline void die(ObjectState* state, Object* object)
    {
        #if DC_CLIENT
        Animations::team_item_explode(state->vox->get_part(0)->get_center(), state->team);
        #endif
        Super::die(state, object);
    }
};

template <class Super, class Object>
class DieRevokeOwner: public Super
{
    protected:
    inline void die(ObjectState* state, Object* object)
    {
        if (state->owner != NO_AGENT)
        {
            Agent_state* a = STATE::agent_list->get(state->owner);
            if (a != NULL)
                a->status.lose_item(state->type);
        }
        Super::die(state, object);
    }
};
