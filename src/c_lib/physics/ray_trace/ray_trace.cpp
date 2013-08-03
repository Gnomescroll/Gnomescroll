#include "ray_trace.hpp"

#include <stdio.h>
#include <math.h>
#include <common/defines.h>
#include <t_map/_interface.hpp>
#include <t_map/t_properties.hpp>
#include <physics/common.hpp>
#include <physics/vec3.hpp>
#include <physics/quadrant.hpp>

static inline bool collision_check(const Vec3i& position)
{
    if (!is_valid_z(position)) return false;
    return t_map::isSolid(translate_position(position));
}

static inline bool collision_check(int x, int y, int z)
{
    return collision_check(vec3i_init(x, y, z));
}

bool raytrace_terrain(const Vec3& start, const Vec3& direction, float length, class RaytraceData* data)
{   // direction must be normalized
    struct Vec3 end = vec3_add(start, vec3_scalar_mult(direction, length));
    return raytrace_terrain(start, end, data);
}

/* Borrowed from: http://playtechs.blogspot.com/2007/03/raytracing-on-grid.html */
bool raytrace_terrain(const Vec3& start, const Vec3& end, class RaytraceData* data)
{
    data->interval = 1.0f;
    if (vec3_equal(start, end))
    {
        if (collision_check(vec3i_init(start)))
        {
            data->set_collision_point(vec3i_init(end));
            return true;
        }
        return false;
    }

    const float dx = fabsf(end.x - start.x);
    const float dy = fabsf(end.y - start.y);
    const float dz = fabsf(end.z - start.z);

    int x = int(floorf(start.x));
    int y = int(floorf(start.y));
    int z = int(floorf(start.z));

    // div by 0 is implicitly handled, will generate +/-infinity
    const float dt_dx = 1.0f / dx;
    const float dt_dy = 1.0f / dy;
    const float dt_dz = 1.0f / dz;

    float t = 0.0f;

    int n = 1;
    int x_inc = 0, y_inc = 0, z_inc = 0;
    float t_next_x = 0.0f, t_next_y = 0.0f, t_next_z = 0.0f;
    int side[3] = {0};

    if (dx == 0)
    {
        x_inc = 0;
        t_next_x = dt_dx; // infinity
    }
    else if (end.x > start.x)
    {
        x_inc = 1;
        n += int(end.x) - x;
        t_next_x = (floorf(start.x) + 1 - start.x) * dt_dx;
    }
    else
    {
        x_inc = -1;
        n += x - int(end.x);
        t_next_x = (start.x - floorf(start.x)) * dt_dx;
    }

    if (dy == 0)
    {
        y_inc = 0;
        t_next_y = dt_dy; // infinity
    }
    else if (end.y > start.y)
    {
        y_inc = 1;
        n += int(end.y) - y;
        t_next_y = (floorf(start.y) + 1 - start.y) * dt_dy;
    }
    else
    {
        y_inc = -1;
        n += y - int(end.y);
        t_next_y = (start.y - floorf(start.y)) * dt_dy;
    }

    if (dz == 0)
    {
        z_inc = 0;
        t_next_z = dt_dz; // infinity
    }
    else if (end.z > start.z)
    {
        z_inc = 1;
        n += int(end.z) - z;
        t_next_z = (floorf(start.z) + 1 - start.z) * dt_dz;
    }
    else
    {
        z_inc = -1;
        n += z - int(floorf(end.z));
        t_next_z = (start.z - floorf(start.z)) * dt_dz;
    }

    // initialize side, incase we're already in a block
    if (t_next_x < t_next_y && t_next_x < t_next_z)
        side[0] = -x_inc;
    else
    if (t_next_y < t_next_z)
        side[1] = -y_inc;
    else
        side[2] = -z_inc;

    for (int i=0; i<n; i++)
    {
        // check for collision;
        // we skip the first block so things don't get stuck
        // nothing should be originating inside the block
        if (i && collision_check(x,y,z))
        {
            if (data != NULL)
            {
                data->side = get_cube_side_from_sides(vec3i_init(side));
                data->interval = GS_MIN(t, 1.0f);
                data->set_collision_point(vec3i_init(x, y, z));
            }
            return true;
        }

        for (int i=0; i<3; side[i++] = 0);

        if (t_next_x < t_next_y && t_next_x < t_next_z)
        {
            x += x_inc;
            x = translate_point(x);
            side[0] = -x_inc;
            t = t_next_x;
            t_next_x += dt_dx;
        }
        else
        if (t_next_y < t_next_z)
        {
            y += y_inc;
            y = translate_point(y);
            side[1] = -y_inc;
            t = t_next_y;
            t_next_y += dt_dy;
        }
        else
        {
            z += z_inc;
            if (z < 0 && z_inc <= 0) break;
            if (z >= map_dim.z && z_inc >= 0) break;
            side[2] = -z_inc;
            t = t_next_z;
            t_next_z += dt_dz;
        }
    }

    return false;
}

inline CubeType RaytraceData::get_cube_type() const
{
    return t_map::get(this->collision_point);
}

inline void RaytraceData::set_collision_point(const Vec3i& position)
{
    this->collision_point = translate_position(position);
}

Vec3i RaytraceData::get_pre_collision_point() const
{
    Vec3i sides = this->get_sides();
    return translate_position(vec3i_add(this->collision_point, sides));
}
