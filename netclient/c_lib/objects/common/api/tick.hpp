#pragma once

#include <c_lib/t_map/t_map.hpp>

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
    Vec3 p = object->get_position();
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
    
    bool changed = object->set_position(p.x, p.y, (float)z);
    if (changed && state->broadcast_state_change)
        object->broadcastState();
    #endif
}

template <class Object>
Agent_state* tickTargetAcquisition(ObjectState* state, Object* object, float camera_z)
{
    #if DC_SERVER
    Agent_state* agent = NULL;
    if (state->fire_tick % state->fire_rate_limit == 0)
        agent = object->acquire_target(
            state->id, state->type, object->get_team(), camera_z,
            object->get_position(),
            state->accuracy_bias, state->sight_range,
            state->attack_enemies, state->attack_random
        );
    state->fire_tick++;
    return agent;
    #endif

    #if DC_CLIENT
    return NULL;
    #endif
}

template <class Object>
void tickRotate(Object* object, float dtheta, float dphi)
{
    object->delta_rotation(dtheta, dphi);
}

// moves vector from origin to dest by momentum amount
Vec3 tickMoveToPoint(Vec3 dest, Vec3 origin, Vec3 momentum)
{
    Vec3 direction = vec3_sub(dest, origin);
    normalize_vector(&direction);
    direction = vec3_mult(direction, momentum);             //  apply magnitude to velocity
    origin = vec3_add(origin, direction);
    return origin;
}

// return x-angle (theta) rotation between two Vec3s
float tickOrientToPointTheta(Vec3 dest, Vec3 origin)
{
    Vec3 direction = vec3_sub(dest, origin);
    direction.z = 0;
    normalize_vector(&direction);

    float t = acos(direction.x) / 3.14159f;
    if (direction.y < 0) t = -t;
    //t += 0.50f; //off by 90 degrees
    return t;
}

//  x- and y-angle (theta,phi) rotation between two Vec3s
void tickOrientToPointThetaPhi(Vec3 dest, Vec3 origin, float* theta, float* phi)
{
    Vec3 direction = vec3_sub(dest, origin);
    Vec3 direction_cached = vec3_copy(direction);
    direction.z = 0;
    normalize_vector(&direction);

    float t = acos(direction.x) / 3.14159f;
    if (direction.y < 0) t = -t;
    //t += 0.50f; //off by 90 degrees
    *theta = t;

    direction_cached.y = 0;
    normalize_vector(&direction_cached);
    if (direction_cached.x < 0) direction_cached.x *= -1;
    t = acos(direction_cached.x) / 3.14159f;
    if (direction_cached.z < 0) t = -t;

    //if (t > 0.5f)
        //t += -0.5f;
    //else if (t < -0.5f)
        //t += 0.5f;

    //if (t < -0.5f) t += 1;
    //if (t > 0.5f) t -= 1;
    //if (t < 0) t += 1;
    //if (t > 1) t -= 1;
    t += 0.50f;
    *phi = t;
}
