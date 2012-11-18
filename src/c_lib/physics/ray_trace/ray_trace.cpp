#include "ray_trace.hpp"

#include <stdio.h>
#include <math.h>

#include <common/defines.h>

#include <t_map/_interface.hpp>
#include <t_map/t_properties.hpp>

#include <physics/common.hpp>

#include <physics/vec3.hpp>
#include <physics/quadrant.hpp>

static inline bool collision_check(int x, int y, int z)
{
    if((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return false;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    return t_map::isSolid(x,y,z);
}

inline float sphere_line_distance(float px, float py, float pz, float ox, float oy, float oz, float tx, float ty, float tz, float* pos, float* _rad2)
{
    if (ox == 0.0f && oy == 0.0f && oz == 0.0f) return t_map::map_dim.x * 4;
    
    tx -= px;
    ty -= py;
    tz -= pz;

    float t = tx*ox + ty*oy + tz*oz; // <tx|ox>

    float d = t/(ox*ox+oy*oy+oz*oz); //distance to collision

    float x = t*ox - tx;
    float y = t*oy - ty;
    float z = t*oz - tz;
    *_rad2 = x*x+y*y+z*z; // minimum distance squared between target and line

    //x,y,z is closest point
    x = t*ox + px;
    y = t*oy + py;
    z = t*oz + pz;

    pos[0] = x;
    pos[1] = y;
    pos[2] = z;

    return d;
}

int get_cube_side_from_side_array(int side[3])
{
    if (side[0] ==  1) return 2;
    if (side[0] == -1) return 3;
    if (side[1] ==  1) return 4;
    if (side[1] == -1) return 5;
    if (side[2] ==  1) return 0;
    if (side[2] == -1) return 1;

    GS_ASSERT(false);
    return 0;
}

void get_side_array_from_cube_side(int side_id, int side[3])
{
    for (int i=0; i<3; i++) side[i] = 0;
    switch (side_id)
    {
        case 2:
            side[0] = 1;
            break;
        case 3:
            side[0] = -1;
            break;
        case 4:
            side[1] = 1;
            break;
        case 5:
            side[1] = -1;
            break;
        case 0:
            side[2] = 1;
            break;
        case 1:
            side[2] = -1;
            break;
        default:
            side[2] = 1;
            GS_ASSERT(false);
            return;
    }
}


bool raytrace_terrain(struct Vec3 start, struct Vec3 direction, float length, struct RaytraceData* data)
{   // direction must be normalized
    struct Vec3 end = vec3_add(start, vec3_scalar_mult(direction, length));
    return raytrace_terrain(start, end, data);
}

/* Borrowed from: http://playtechs.blogspot.com/2007/03/raytracing-on-grid.html */
bool raytrace_terrain(struct Vec3 start, struct Vec3 end, struct RaytraceData* data)
{
    if (vec3_equal(start, end))
    {
        if (collision_check(start.x, start.y, start.z))
        {
            data->set_collision_point(start.x, start.y, start.z);
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
                data->side = get_cube_side_from_side_array(side);
                data->interval = GS_MIN(t, 1.0f);
                data->set_collision_point(x,y,z);
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
            if (z >= t_map::map_dim.z && z_inc >= 0) break;
            side[2] = -z_inc;
            t = t_next_z;
            t_next_z += dt_dz;
        }
    }
    
    return false;
}

inline const CubeID RaytraceData::get_cube_id()
{
    return t_map::get(this->collision_point[0], this->collision_point[1], this->collision_point[2]); 
}

inline void RaytraceData::set_collision_point(int x, int y, int z)
{
    this->collision_point[0] = translate_point(x);
    this->collision_point[1] = translate_point(y);
    this->collision_point[2] = z;
}

const void RaytraceData::get_pre_collision_point(int pre_collision_point[3])
{
    int sides[3];
    this->get_side_array(sides);
    for (int i=0; i<3; i++)
        pre_collision_point[i] = collision_point[i] + sides[i];
    pre_collision_point[0] = translate_point(pre_collision_point[0]);
    pre_collision_point[1] = translate_point(pre_collision_point[1]);
}
