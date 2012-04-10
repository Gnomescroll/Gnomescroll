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
{   // TODO -- this may be incorrect
    Vec3 direction = vec3_sub(dest, origin);
    direction.z = 0;
    normalize_vector(&direction);
    return acos(direction.x);
}

//  x- and y-angle (theta,phi) rotation between two Vec3s
void tickOrientToPointThetaPhi(Vec3 dest, Vec3 origin, float* theta, float* phi)
{
    //float x = dest.x - origin.x;
    //float y = dest.y - origin.y;
    //float _l = sqrt(x*x + y*y);
    //x /= _l;
    //y /= _l;

    //float _x = 1.0;
    //float _y = 0.0; 

    //float t = acos(_x*x + _y*y) / 3.141519;
    //if ((_x*y - _y*x) < 0) t = -t;

    //t += 0.50; //off by 90 degrees
    //*theta = t;
    
    Vec3 direction = vec3_sub(dest, origin);
    direction.z = 0;
    normalize_vector(&direction);

    float t = acos(direction.x) / 3.141519;
    if (direction.y < 0) t = -t;
    t += 0.50; //off by 90 degrees
    *theta = t;

    *phi = 0;
}
