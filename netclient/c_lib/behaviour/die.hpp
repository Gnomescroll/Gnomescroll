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
}
#endif

template <class Super, class Object>
class DieExplode
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
    }
};
