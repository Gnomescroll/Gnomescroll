#include "agent_physics.hpp"

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

bool agent_collides_terrain(Agent_state* a)
{
    float h = a->current_height();
    return collision_check_final_current(a->box.box_r, h, a->s.x, a->s.y, a->s.z);
}

#define GROUND_MARGIN 0.03f
// checks the (agent bottom - margin) at 4 corners of the agent
inline bool on_ground(float box_r, float x, float y, float z)
{
    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    int zz = z - GROUND_MARGIN;

    if(isSolid(_get(x_max,y_max,zz) != 0)) //north, west
        return true;
    if(isSolid(_get(x_max,y_min,zz) != 0)) //north, east
        return true;
    if(isSolid(_get(x_min,y_min,zz) != 0)) //south, east
        return true;
    if(isSolid(_get(x_min,y_max,zz) != 0)) //south, west
        return true;

    return false;
}
#undef GROUND_MARGIN

inline bool can_stand_up(float box_r, float box_h, float x, float y, float z)
{
    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    int n_z = (int)ceil(box_h);

    for (int i=0; i<n_z; i++)
    {
        int zz = (int)z + i;
        if (i == n_z-1)
            zz = (int)(z + box_h);
        
        if(isSolid(_get(x_max,y_max,zz) != 0)) //north, west
            return true;
        if(isSolid(_get(x_max,y_min,zz) != 0)) //north, east
            return true;
        if(isSolid(_get(x_min,y_min,zz) != 0)) //south, east
            return true;
        if(isSolid(_get(x_min,y_max,zz) != 0)) //south, west
            return true;
    }
    return false;
}

inline bool collision_check_final_current(float box_r, float box_h, float x, float y, float z)
{
    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    const int steps = 6;    // CALIBRATED TO AGENT'S HEIGHT SETTINGS. AD HOC
    const float step_size = box_h / ((float)steps);

    for (int i=0; i<steps; i++)
    {
        int zz = (int)(z + i*step_size);

        if(isSolid(_get(x_max,y_max,zz) != 0)) //north, west
            return true;
        if(isSolid(_get(x_max,y_min,zz) != 0)) //north, east
            return true;
        if(isSolid(_get(x_min,y_min,zz) != 0)) //south, east
            return true;
        if(isSolid(_get(x_min,y_max,zz) != 0)) //south, west
            return true;  
    }
    return false;
}

inline bool collision_check_final_xy(float box_r, float box_h, float x, float y, float z)
{
    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    const int steps = 6;
    const float top_margin = 0.01f;
    const float step_size = (box_h - top_margin) / ((float)steps);

    for (int i=0; i<steps+1; i++)
    {
        int zz = (int)(z + i*step_size);

        if(isSolid(_get(x_max,y_max,zz) != 0)) //north, west
            return true;
        if(isSolid(_get(x_max,y_min,zz) != 0)) //north, east
            return true;
        if(isSolid(_get(x_min,y_min,zz) != 0)) //south, east
            return true;
        if(isSolid(_get(x_min,y_max,zz) != 0)) //south, west
            return true;      
    }
    return false;
}

inline bool collision_check_final_z(float box_r, float box_h, float x, float y, float z, bool* top)
{
    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    const float step_size = 0.9f;
    int steps = (int)ceil(box_h/step_size);

    *top=false;
    for (int i=0; i<=steps; i++)
    {
        int zz = (int)(z + i*step_size);

        if (i*step_size >= box_h)
            *top=true;

        if (zz > (z+box_h))
            zz = (int)(z + box_h);

        if(isSolid(_get(x_max,y_max,zz) != 0)) //north, west
            return true;
        if(isSolid(_get(x_max,y_min,zz) != 0)) //north, east
            return true;
        if(isSolid(_get(x_min,y_min,zz) != 0)) //south, east
            return true;
        if(isSolid(_get(x_min,y_max,zz) != 0)) //south, west
            return true;       
    }
    return false;
}
