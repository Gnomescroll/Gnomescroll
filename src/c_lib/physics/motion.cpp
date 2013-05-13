#include "motion.hpp"

#include <math.h>
#include <t_map/_interface.hpp>

int stick_to_terrain_surface(Vec3 position)
{   // fall/climb with terrain
    #if DC_SERVER
    int x = position.x;
    int y = position.y;
    int z = position.z;
    if (t_map::isSolid(t_map::get(x,y,z)))
    {   // occupied block is solid, move up
        while (t_map::isSolid(t_map::get(x,y,++z)))
            if (z >= map_dim.z)
            {
                z = map_dim.z;
                break;
            }
    }
    else
    {   // occupied block is empty, fall down
        while (!t_map::isSolid(t_map::get(x,y,--z)))
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

Vec3 move_to_point(Vec3 dest, struct Vec3 origin, struct Vec3 momentum)
{   // moves vector from origin to dest by momentum amount
    dest = quadrant_translate_position(origin, dest);
    if (vec3_equal(dest, origin)) return origin;
    Vec3 direction = vec3_sub(dest, origin);
    normalize_vector(&direction);
    direction = vec3_mult(direction, momentum);  // apply magnitude to velocity
    origin = vec3_add(origin, direction);
    return origin;
}

float orient_to_point(Vec3 dest, struct Vec3 origin)
{   // return x-angle (theta) rotation between two Vec3s
    dest = quadrant_translate_position(origin, dest);
    Vec3 direction = vec3_sub(dest, origin);
    return vec3_to_theta(direction);
}

void orient_to_point(Vec3 dest, struct Vec3 origin, float* theta, float* phi)
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
static bool advance_move(struct Vec3 position, struct Vec3 move_to, float speed,
                          struct Vec3* new_position, struct Vec3* new_momentum)
{   // TODO -- fix this function for falling.
    const float GRAVITY = -0.15f;
    const float CLIMB = 0.05f;
    Vec3 new_direction = vec3_init(0);
    if (position.z > move_to.z)
    {
        *new_momentum.z = GRAVITY;
        position.z += GRAVITY;
        position.z = GS_MAX(position.z, move_to.z);
    }
    else if (position.z < move_to.z)
    {
        *new_momentum.z = CLIMB;
        position.z += CLIMB;
        position.z = GS_MIN(position.z, move_to.z);
    }

    float position_z = position.z;
    float move_to_z = move_to.z;
    move_to.z = 0;
    position.z = 0;
    if (vec3_equal(position, move_to) || speed == 0.0f)
    {
        *new_momentum = new_direction;
        *new_position = translate_position(position);
        return (position_z != move_to_z);
    }

    new_direction = vec3_sub(move_to, position);
    new_direction = vec3_normalize(new_direction);
    Vec3 m = vec3_scalar_mult(new_direction, speed);
    position = vec3_add(position, m);
    position.z = position_z;
    *new_momentum = m;
    *new_position = translate_position(position);
    return true;
}

static bool move_z_diff(struct Vec3 position, struct Vec3 move_to,
                         int z, float speed, float max_z_diff,
                         struct Vec3* new_position, struct Vec3* new_momentum)
{
    float z_diff = float(z) - position.z;
    //if (fabsf(z_diff) > max_z_diff)
    if (z_diff > max_z_diff)
    {   // cant move up
        *new_position = position;
        *new_momentum = vec3_init(0);
        return false;
    }
    move_to.z = z;
    return advance_move(position, move_to, z, speed, new_position, new_momentum);
}

bool move_along_terrain_surface(struct Vec3 position, struct Vec3 direction,
                                float speed, float max_z_diff,
                                struct Vec3* new_position, struct Vec3* new_momentum)
{
    // attempt to move to location defined by direction * speed
    // if z_level diff too high, set momentum 0, copy position, return
    // else calculate new destination, reorient direction, multiply by speed, and set to new_momentum
    // add new_momentum to position to get new_position
    // assumes direction is normalized

    GS_ASSERT_LIMIT(speed > 0.0f, 50);
    GS_ASSERT_LIMIT(vec3_length_squared(direction) > 0.0f, 50);
    if (vec3_length_squared(direction) == 0.0f) return false;

    Vec3 move_to = vec3_add(position, vec3_scalar_mult(direction, speed));
    int z = t_map::get_highest_open_block(translate_point(move_to.x), translate_point(move_to.y));

    return move_z_diff(position, move_to, z, speed, max_z_diff, new_position, new_momentum);
}

bool move_along_terrain_surface(struct Vec3 position, struct Vec3 direction,
                                float speed, float max_z_diff,
                                struct Vec3* new_position, struct Vec3* new_momentum,
                                int object_height)
{
    // attempt to move to location defined by direction * speed
    // if z_level diff too high, set momentum 0, copy position, return
    // else calculate new destination, reorient direction, multiply by speed, and set to new_momentum
    // add new_momentum to position to get new_position
    // assumes direction is normalized

    GS_ASSERT_LIMIT(speed > 0.0f, 50);
    GS_ASSERT_LIMIT(vec3_length_squared(direction) > 0.0f, 50);

    Vec3 move_to = vec3_add(position, vec3_scalar_mult(direction, speed));
    // WARNING: this allows things to slip through terrain. need a different method
    int z = t_map::get_highest_open_block(translate_point(move_to.x),
                                          translate_point(move_to.y),
                                          object_height);
    return move_z_diff(position, move_to, z, speed, max_z_diff, new_position, new_momentum);
}

/* Advances position in direction by speed (in meters/tick)
 * returns false if position was left unchanged
 *
 * This version ignores terrain z level differences; it will move up or down any z difference automatically
 */
bool move_along_terrain_surface(struct Vec3 position, struct Vec3 direction,
                                float speed, struct Vec3* new_position,
                                struct Vec3* new_momentum)
{
    // attempt to move to location defined by direction * speed
    // if z_level diff too high, set momentum 0, copy position, return
    // else calculate new destination, reorient direction, multiply by speed, and set to new_momentum
    // add new_momentum to position to get new_position
    // assumes direction is normalized

    GS_ASSERT_LIMIT(speed > 0.0f, 50);
    GS_ASSERT_LIMIT(vec3_length_squared(direction) > 0.0f, 50);

    Vec3 move_to = vec3_add(position, vec3_scalar_mult(direction, speed));
    int z = t_map::get_highest_open_block(translate_point(move_to.x), translate_point(move_to.y));
    move_to.z = z;

    return advance_move(position, move_to, z, speed, new_position, new_momentum);
}

bool move_along_terrain_surface(struct Vec3 position, struct Vec3 direction,
                                float speed, struct Vec3* new_position,
                                struct Vec3* new_momentum, int object_height)
{
    // attempt to move to location defined by direction * speed
    // if z_level diff too high, set momentum 0, copy position, return
    // else calculate new destination, reorient direction, multiply by speed, and set to new_momentum
    // add new_momentum to position to get new_position
    // assumes direction is normalized

    GS_ASSERT_LIMIT(speed > 0.0f, 50);
    GS_ASSERT_LIMIT(vec3_length_squared(direction) > 0.0f, 50);

    Vec3 move_to = vec3_add(position, vec3_scalar_mult(direction, speed));
    // WARNING: this allows things to slip through terrain. need a different method
    int z = t_map::get_highest_open_block(translate_point(move_to.x),
                                          translate_point(move_to.y),
                                          object_height);
    move_to.z = z;
    return advance_move(position, move_to, z, speed, new_position, new_momentum);
}


// These will move between blocks if it makes sense to

bool move_within_terrain_surface(struct Vec3 position, struct Vec3 direction,
                                 float speed, float max_z_diff,
                                 struct Vec3* new_position, struct Vec3* new_momentum)
{
    // attempt to move to location defined by direction * speed
    // if z_level diff too high, set momentum 0, copy position, return
    // else calculate new destination, reorient direction, multiply by speed, and set to new_momentum
    // add new_momentum to position to get new_position
    // assumes direction is normalized

    GS_ASSERT_LIMIT(speed > 0.0f, 50);
    GS_ASSERT_LIMIT(vec3_length_squared(direction) > 0.0f, 50);
    if (vec3_length_squared(direction) == 0.0f) return false;

    Vec3 move_to = vec3_add(position, vec3_scalar_mult(direction, speed));
    int z = t_map::get_nearest_surface_block(translate_point(move_to.x),
                                             translate_point(move_to.y),
                                             move_to.z);

    return move_z_diff(position, move_to, z, speed, max_z_diff, new_position, new_momentum);
}

bool move_within_terrain_surface(struct Vec3 position, struct Vec3 direction,
                                 float speed, float max_z_diff,
                                 struct Vec3* new_position, struct Vec3* new_momentum,
                                 int object_height)
{
    // attempt to move to location defined by direction * speed
    // if z_level diff too high, set momentum 0, copy position, return
    // else calculate new destination, reorient direction, multiply by speed, and set to new_momentum
    // add new_momentum to position to get new_position
    // assumes direction is normalized

    GS_ASSERT_LIMIT(speed > 0.0f, 50);
    GS_ASSERT_LIMIT(vec3_length_squared(direction) > 0.0f, 50);
    if (vec3_length_squared(direction) == 0.0f) return false;

    Vec3 move_to = vec3_add(position, vec3_scalar_mult(direction, speed));
    int z = t_map::get_nearest_surface_block(translate_point(move_to.x),
                                             translate_point(move_to.y),
                                             move_to.z, object_height);
    return move_z_diff(position, move_to, z, speed, max_z_diff, new_position, new_momentum);
}

#undef FLOAT_ERROR_MARGIN
