#pragma once

#include <c_lib/physics/vec3.hpp>
#include <c_lib/t_map/t_map.hpp>

void tickTTL(ObjectState* state)
{   // increment ttl
    if (state->ttl >= 0)
        state->ttl++;
}

int tickStayOnGround(ObjectState* state, Vec3 position)
{   // fall/climb with terrain
    #if DC_SERVER
    int x = (int)position.x;
    int y = (int)position.y;
    int z = (int)position.z;
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
    return z;
    #endif

    #if DC_CLIENT
    return position.z;
    #endif
}

Vec3 tickMoveToPoint(Vec3 dest, Vec3 origin, Vec3 momentum)
{   // moves vector from origin to dest by momentum amount
    Vec3 direction = vec3_sub(dest, origin);
    normalize_vector(&direction);
    direction = vec3_mult(direction, momentum);             //  apply magnitude to velocity
    origin = vec3_add(origin, direction);
    return origin;
}

float tickOrientToPointTheta(Vec3 dest, Vec3 origin)
{   // return x-angle (theta) rotation between two Vec3s
    Vec3 direction = vec3_sub(dest, origin);
    return vec3_to_theta(direction);
}

void tickOrientToPointThetaPhi(Vec3 dest, Vec3 origin, float* theta, float* phi)
{   //  x- and y-angle (theta,phi) rotation between two Vec3s
    Vec3 direction = vec3_sub(dest, origin);
    vec3_to_angles(direction, theta, phi);
}
