#include "motion.hpp"

#include <math.h>
#include <t_map/t_map.hpp>

int stick_to_terrain_surface(Vec3 position)
{   // fall/climb with terrain
    #if DC_SERVER
    int x = (int)position.x;
    int y = (int)position.y;
    int z = (int)position.z;
    if (isSolid(t_map::get(x,y,z)))
    {   // occupied block is solid, move up
        while (isSolid(t_map::get(x,y,++z)))
            if (z >= map_dim.z)
            {
                z = map_dim.z;
                break;
            }
    }
    else
    {   // occupied block is empty, fall down
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

Vec3 move_to_point(Vec3 dest, Vec3 origin, Vec3 momentum)
{   // moves vector from origin to dest by momentum amount
    dest = quadrant_translate_position(origin, dest);
    Vec3 direction = vec3_sub(dest, origin);
    normalize_vector(&direction);
    direction = vec3_mult(direction, momentum);             //  apply magnitude to velocity
    origin = vec3_add(origin, direction);
    return origin;
}

float orient_to_point(Vec3 dest, Vec3 origin)
{   // return x-angle (theta) rotation between two Vec3s
    dest = quadrant_translate_position(origin, dest);
    Vec3 direction = vec3_sub(dest, origin);
    return vec3_to_theta(direction);
}

void orient_to_point(Vec3 dest, Vec3 origin, float* theta, float* phi)
{   //  x- and y-angle (theta,phi) rotation between two Vec3s
    dest = quadrant_translate_position(origin, dest);
    Vec3 direction = vec3_sub(dest, origin);
    vec3_to_angles(direction, theta, phi);
}

/* Advances position in direction by speed (in meters/tick)
 *  max_z_down and max_z_up indicate the maximum values for climbing/falling
 *
 * returns false if position was left unchanged
 */
 
#define FLOAT_ERROR_MARGIN 0.005f
static bool advance_move(Vec3 position, Vec3 move_to, int z, float speed, Vec3* new_position, Vec3* new_momentum)
{
    Vec3 new_direction = vec3_sub(move_to, position);
    float len = vec3_length_squared(new_direction);
    if (len < FLOAT_ERROR_MARGIN)
    {
        new_direction = vec3_init(0,0,0);
        len = 0.0f;
    }
    else normalize_vector(&new_direction);
    GS_ASSERT(len < 512.0f*512.0f);

    *new_momentum = vec3_scalar_mult(new_direction, speed);
    position = vec3_add(position, *new_momentum);

    new_direction.z = 0;
    float xy_len = vec3_length_squared(new_direction);
    if (xy_len < (speed*speed)/2) position.z = z;
    *new_position = translate_position(position);
    
    return true;    
}
 
static bool move_z_diff(Vec3 position, Vec3 move_to, int z, float speed, float max_z_diff, Vec3* new_position, Vec3* new_momentum)
{    
    float z_diff = position.z - ((float)z);
    if (fabsf(z_diff) > max_z_diff)
    {   // cant move
        *new_position = position;
        *new_momentum = vec3_init(0,0,0);
        return false;
    }
    move_to.z = z;
    
    return advance_move(position, move_to, z, speed, new_position, new_momentum);
}
 
bool move_along_terrain_surface(Vec3 position, Vec3 direction, float speed, float max_z_diff, Vec3* new_position, Vec3* new_momentum)
{
    // attempt to move to location defined by direction * speed
    // if z_level diff too high, set momentum 0, copy position, return
    // else calculate new destination, reorient direction, multiply by speed, and set to new_momentum
    // add new_momentum to position to get new_position
    // assumes direction is normalized

    GS_ASSERT(speed > 0.0f);
    GS_ASSERT(vec3_length(direction) > 0.0f);

    Vec3 move_to = vec3_add(position, vec3_scalar_mult(direction, speed));
    int z = t_map::get_highest_open_block(translate_point(move_to.x), translate_point(move_to.y));

    return move_z_diff(position, move_to, z, speed, max_z_diff, new_position, new_momentum);
}

bool move_along_terrain_surface(Vec3 position, Vec3 direction, float speed, float max_z_diff, Vec3* new_position, Vec3* new_momentum, int object_height)
{
    // attempt to move to location defined by direction * speed
    // if z_level diff too high, set momentum 0, copy position, return
    // else calculate new destination, reorient direction, multiply by speed, and set to new_momentum
    // add new_momentum to position to get new_position
    // assumes direction is normalized

    GS_ASSERT(speed > 0.0f);
    GS_ASSERT(vec3_length(direction) > 0.0f);

    Vec3 move_to = vec3_add(position, vec3_scalar_mult(direction, speed));
    int z = t_map::get_nearest_open_block(translate_point(move_to.x), translate_point(move_to.y), move_to.z, object_height);

    return move_z_diff(position, move_to, z, speed, max_z_diff, new_position, new_momentum);
}

/* Advances position in direction by speed (in meters/tick)
 * returns false if position was left unchanged
 *
 * This version ignores terrain z level differences
 */
bool move_along_terrain_surface(Vec3 position, Vec3 direction, float speed, Vec3* new_position, Vec3* new_momentum)
{
    // attempt to move to location defined by direction * speed
    // if z_level diff too high, set momentum 0, copy position, return
    // else calculate new destination, reorient direction, multiply by speed, and set to new_momentum
    // add new_momentum to position to get new_position
    // assumes direction is normalized

    GS_ASSERT(speed > 0.0f);
    GS_ASSERT(vec3_length(direction) > 0.0f);

    Vec3 move_to = vec3_add(position, vec3_scalar_mult(direction, speed));
    int z = t_map::get_highest_open_block(translate_point(move_to.x), translate_point(move_to.y));
    move_to.z = z;

    return advance_move(position, move_to, z, speed, new_position, new_momentum);
}

bool move_along_terrain_surface(Vec3 position, Vec3 direction, float speed, Vec3* new_position, Vec3* new_momentum, int object_height)
{
    // attempt to move to location defined by direction * speed
    // if z_level diff too high, set momentum 0, copy position, return
    // else calculate new destination, reorient direction, multiply by speed, and set to new_momentum
    // add new_momentum to position to get new_position
    // assumes direction is normalized

    GS_ASSERT(speed > 0.0f);
    GS_ASSERT(vec3_length(direction) > 0.0f);

    Vec3 move_to = vec3_add(position, vec3_scalar_mult(direction, speed));
    int z = t_map::get_nearest_open_block(translate_point(move_to.x), translate_point(move_to.y), move_to.z, object_height);
    move_to.z = z;

    return advance_move(position, move_to, z, speed, new_position, new_momentum);
}
#undef FLOAT_ERROR_MARGIN
