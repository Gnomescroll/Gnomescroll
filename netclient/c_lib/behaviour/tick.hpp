#pragma once

#include <c_lib/t_map/t_map.hpp>

template <class Object>
void tickVerletBounce(ObjectState* state, Object* object)
{
    Verlet::bounce(state->vp, state->damp);
}

template <class Object>
void tickTTL(ObjectState* state, Object* object)
{
    if (state->ttl >= 0)
        state->ttl++;
}

template <class Object>
void tickStayOnGround(ObjectState* state, Object* object)
{
    #if DC_SERVER
    // fall/climb with terrain
    Vec3 p = state->get_position();
    int x = (int)p.x;
    int y = (int)p.y;
    int z = (int)p.z;
    if (isSolid(t_map::get(x,y,z)))
    {   // move up
        while (isSolid(t_map::get(x,y,++z)))
            if (z >= map_dim.z)
            {
                z = map_dim.z;
                break;
            }
    }
    else
    {   // fall down
        while (!isSolid(t_map::get(x,y,--z)))
            if (z<=0)
            {
                z = 0;
                break;
            }
        z++;
    }
    
    bool changed = state->set_position(p.x, p.y, (float)z);
    if (changed && state->broadcast_state_change)
        object->broadcastState();
    #endif
}

template <class Object>
void tickTargetAcquisition(ObjectState* state, Object* object)
{
    #if DC_SERVER
    if (state->fire_tick % state->fire_rate_limit == 0)
        object->acquire_target(
            state->id, state->type, object->get_team(), state->camera_z(),
            state->get_position(),
            state->accuracy_bias, state->sight_range,
            state->attack_enemies, state->attack_random
        );
    state->fire_tick++;
    #endif
}
