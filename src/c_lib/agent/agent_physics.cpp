#include "agent_physics.hpp"

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

bool object_collides_terrain(Vec3 position, float height, float radius)
{
    ASSERT_BOXED_POSITION(position);
    return Agents::collision_check_final_current(radius, height, position.x, position.y, position.z);
}

namespace Agents
{

bool agent_collides_terrain(Agent* a)
{
    float h = a->current_height();
    Vec3 p = a->get_position();
    return collision_check_final_current(a->box.box_r, h, p.x, p.y, p.z);
}

#define GROUND_MARGIN 0.03f
// checks the (agent bottom - margin) at 4 corners of the agent
inline bool on_ground(float box_r, float x, float y, float z)
{
    float x_min = x - box_r;
    float x_max = x + box_r;

    float y_min = y - box_r;
    float y_max = y + box_r;

    x_min = translate_point(x_min);
    x_max = translate_point(x_max);
    y_min = translate_point(y_min);
    y_max = translate_point(y_max);

    int zz = z - GROUND_MARGIN;

    if (t_map::isSolid(x_max,y_max,zz) || //north, west
         t_map::isSolid(x_max,y_min,zz) || //north, east
         t_map::isSolid(x_min,y_min,zz) || //south, east
         t_map::isSolid(x_min,y_max,zz))   //south, west
        return true;
    return false;
}
#undef GROUND_MARGIN

inline bool can_stand_up(float box_r, float box_h, float x, float y, float z)
{
    float x_min = x - box_r;
    float x_max = x + box_r;

    float y_min = y - box_r;
    float y_max = y + box_r;
    
    x_min = translate_point(x_min);
    x_max = translate_point(x_max);
    y_min = translate_point(y_min);
    y_max = translate_point(y_max);

    int n_z = (int)ceil(box_h);

    for (int i=0; i<n_z; i++)
    {
        int zz = (int)z + i;
        if (i == n_z-1)
            zz = (int)(z + box_h);

        if( t_map::isSolid(x_max,y_max,zz) || //north, west
            t_map::isSolid(x_max,y_min,zz) || //north, east
            t_map::isSolid(x_min,y_min,zz) || //south, east
            t_map::isSolid(x_min,y_max,zz) )  //south, west
        return true;
    }
    return false;
}

inline bool collision_check_final_current(float box_r, float box_h, float x, float y, float z)
{
    float x_min = x - box_r;
    float x_max = x + box_r;

    float y_min = y - box_r;
    float y_max = y + box_r;

    x_min = translate_point(x_min);
    x_max = translate_point(x_max);
    y_min = translate_point(y_min);
    y_max = translate_point(y_max);

    const int steps = 6;    // CALIBRATED TO AGENT'S HEIGHT SETTINGS. AD HOC
    const float step_size = box_h / ((float)steps);

    for (int i=0; i<steps; i++)
    {
        int zz = (int)(z + i*step_size);

        if( t_map::isSolid(x_max,y_max,zz) || //north, west
            t_map::isSolid(x_max,y_min,zz) || //north, east
            t_map::isSolid(x_min,y_min,zz) || //south, east
            t_map::isSolid(x_min,y_max,zz) )  //south, west
        return true;
    }
    return false;
}

inline bool collision_check_final_xy(float box_r, float box_h, float x, float y, float z)
{
    float x_min = x - box_r;
    float x_max = x + box_r;

    float y_min = y - box_r;
    float y_max = y + box_r;

    x_min = translate_point(x_min);
    x_max = translate_point(x_max);
    y_min = translate_point(y_min);
    y_max = translate_point(y_max);

    const int steps = 6;
    const float top_margin = 0.01f;
    const float step_size = (box_h - top_margin) / ((float)steps);

    for (int i=0; i<steps+1; i++)
    {
        int zz = (int)(z + i*step_size);

        if( t_map::isSolid(x_max,y_max,zz) || //north, west
            t_map::isSolid(x_max,y_min,zz) || //north, east
            t_map::isSolid(x_min,y_min,zz) || //south, east
            t_map::isSolid(x_min,y_max,zz) )  //south, west
        return true;   
    }
    return false;
}

inline bool collision_check_final_z(float box_r, float box_h, float x, float y, float z, bool* top)
{
    float x_min = x - box_r;
    float x_max = x + box_r;

    float y_min = y - box_r;
    float y_max = y + box_r;

    x_min = translate_point(x_min);
    x_max = translate_point(x_max);
    y_min = translate_point(y_min);
    y_max = translate_point(y_max);

    const float step_size = 0.9f;
    int steps = (int)ceil(box_h/step_size);

    *top = false;
    for (int i=0; i<=steps; i++)
    {
        int zz = (int)(z + i*step_size);

        if (i*step_size >= box_h)
            *top=true;

        if (zz > (z+box_h))
            zz = (int)(z + box_h);

        if(t_map::isSolid(x_max,y_max,zz) || //north, west
            t_map::isSolid(x_max,y_min,zz) || //north, east
            t_map::isSolid(x_min,y_min,zz) || //south, east
            t_map::isSolid(x_min,y_max,zz))  //south, west
            return true;
    }

    return false;
}

}   // Agents
