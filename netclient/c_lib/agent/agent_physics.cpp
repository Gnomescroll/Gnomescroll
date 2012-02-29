#include "agent_physics.hpp"

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <math.h>

// assume box_r < 1
inline void collision_check1(float box_r, float box_h, float x, float y, float z, int collision[6])
{
    //north +x
    //south -x
    //west +y
    //east -y
    //top +z
    //bottom -z
    int i;
    for(i=0; i<6; i++) collision[i] = 0;

    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    int z0 = z;
    int z1 = z+1.0f;
    int z2 = z+box_h;

    //upper left
    //upper right
    //bottom right
    //bottom left
    if(isActive(_get(x_max,y_max,z0) != 0)) {
        //north, west, bottom
        collision[0]++; //north 
        collision[2]++; //west
        collision[5]++; //bottom
    }
    if(isActive(_get(x_max,y_max,z1) != 0)) {
        //north, west
        collision[0]++; //north 
        collision[2]++; //west
    }
    if(isActive(_get(x_max,y_max,z2) != 0)) {
        //north, west, top
        collision[0]++; //north 
        collision[2]++; //west
        collision[4]++; //top     
    }


    if(isActive(_get(x_max,y_min,z0) != 0)) {
        //north, east, bottom
        collision[0]++; //north 
        collision[3]++; //east 
        collision[5]++; //bottom
    }
    if(isActive(_get(x_max,y_min,z1) != 0)) {
        //north, east
        collision[0]++; //north 
        collision[3]++; //east 
    }
    if(isActive(_get(x_max,y_min,z2) != 0)) {
        //north, east, top
        collision[0]++; //north 
        collision[3]++; //east
        collision[4]++; //top
    }


    if(isActive(_get(x_min,y_min,z0) != 0)) {
        //south, east, bottom
        collision[1]++; //south
        collision[3]++; //east
        collision[5]++; //bottom
    }
    if(isActive(_get(x_min,y_min,z1) != 0)) {
        //south, east
        collision[1]++; //south
        collision[3]++; //east 
    }
    if(isActive(_get(x_min,y_min,z2) != 0)) {
        //south, east, top
        collision[1]++; //south
        collision[3]++; //east 
        collision[4]++; //top
    }

    if(isActive(_get(x_min,y_max,z0) != 0)) {
        //south, west
        collision[1]++; //south
        collision[2]++; //west
        collision[5]++; //bottom
    }
    if(isActive(_get(x_min,y_max,z1) != 0)) {
        //south, west
        collision[1]++; //south
        collision[2]++; //west
    }
    if(isActive(_get(x_min,y_max,z2) != 0)) {
        //south, west
        collision[1]++; //south
        collision[2]++; //west
        collision[4]++; //top
    }

    //printf("collision: n=%i, s=%i, w=%i, e=%i, t=%i, b=%i \n", collision[0],collision[1],collision[2],collision[3],collision[4],collision[5] );
}

inline bool collision_check2(float box_r, float box_h, float x, float y, float z)
{
    //north +x
    //south -x
    //west +y
    //east -y
    //top +z
    //bottom -z

    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    int z0 = z;
    int z1 = z + 1.0f;
    int z2 = z + box_h;

    //upper left
    //upper right
    //bottom right
    //bottom left
    if(isActive(_get(x_max,y_max,z0) != 0) || isActive(_get(x_max,y_max,z1) != 0) || isActive(_get(x_max,y_max,z2) != 0) ) {
        //north, west
        return true;
    }


    if(isActive(_get(x_max,y_min,z0) != 0) || isActive(_get(x_max,y_min,z1) != 0) || isActive(_get(x_max,y_min,z2) != 0) ) {
        //north, east
        return true;
    }

    if(isActive(_get(x_min,y_min,z0) != 0) || isActive(_get(x_min,y_min,z1) != 0) || isActive(_get(x_min,y_min,z2) != 0) ) {
        //south, east
        return true;
    }

    if(isActive(_get(x_min,y_max,z0) != 0) || isActive(_get(x_min,y_max,z1) != 0) || isActive(_get(x_min,y_max,z2) != 0) ) {
        //south, west
        return true;
    }

    return false;
}

bool agent_collides_terrain(Agent_state* a)
{
    //return collision_check5(a->box.box_r, a->box.b_height, a->s.x, a->s.y, a->s.z);
    //return collision_check5_stand_up(a->box.box_r, a->box.b_height, a->s.x, a->s.y, a->s.z);
    float h = a->current_height();
    return collision_check_final_current(a->box.box_r, h, a->s.x, a->s.y, a->s.z);
}

// for when box_h < 1
inline bool collision_check_short(float box_r, float box_h, float x, float y, float z)
{
    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    int z0 = z;
    int z1 = z + box_h;

    if(isActive(_get(x_max,y_max,z0) != 0) || isActive(_get(x_max,y_max,z1) != 0)) {
        //north, west
        return true;
    }

    if(isActive(_get(x_max,y_min,z0) != 0) || isActive(_get(x_max,y_min,z1) != 0)) {
        //north, east
        return true;
    }

    if(isActive(_get(x_min,y_min,z0) != 0) || isActive(_get(x_min,y_min,z1) != 0)) {
        //south, east
        return true;
    }

    if(isActive(_get(x_min,y_max,z0) != 0) || isActive(_get(x_min,y_max,z1) != 0)) {
        //south, west
        return true;
    }

    return false;
}

inline bool collision_check4(float box_r, float box_h, float x, float y, float z, int *cx, int *cy, int *cz)
// collision_check2 + fills in int pointers with colliding block
{
    //north +x
    //south -x
    //west +y
    //east -y
    //top +z
    //bottom -z

    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    int z0 = z;
    int z1 = z + 1.0f;
    int z2 = z + box_h;

    //upper left
    //upper right
    //bottom right
    //bottom left
    if(isActive(_get(x_max,y_max,z0) != 0) || isActive(_get(x_max,y_max,z1) != 0) || isActive(_get(x_max,y_max,z2) != 0) ) {
        //north, west
        *cx = x_max;
        *cy = y_max;
        *cz = z0;
        return true;
    }


    if(isActive(_get(x_max,y_min,z0) != 0) || isActive(_get(x_max,y_min,z1) != 0) || isActive(_get(x_max,y_min,z2) != 0) ) {
        //north, east
        *cx = x_max;
        *cy = y_min;
        *cz = z0;
        return true;
    }

    if(isActive(_get(x_min,y_min,z0) != 0) || isActive(_get(x_min,y_min,z1) != 0) || isActive(_get(x_min,y_min,z2) != 0) ) {
        //south, east
        *cx = x_min;
        *cy = y_min;
        *cz = z0;
        return true;
    }

    if(isActive(_get(x_min,y_max,z0) != 0) || isActive(_get(x_min,y_max,z1) != 0) || isActive(_get(x_min,y_max,z2) != 0) ) {
        //south, west
        *cx = x_min;
        *cy = y_max;
        *cz = z0;
        return true;
    }

    return false;
}

#define GROUND_MARGIN 0.10f
// checks the (agent bottom - margin) at 4 corners of the agent
inline bool on_ground(float box_r, float x, float y, float z)
{

    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    int z0 = z - GROUND_MARGIN;

    //upper left
    //upper right
    //bottom right
    //bottom left

    //in future, check tile height if collision
    if(isActive(_get(x_max,y_max,z0))) {
        return true;
    }

    if(isActive(_get(x_max,y_min,z0))) {
        return true; 
    }

    if(isActive(_get(x_min,y_min,z0))) {
        return true;
    }

    if(isActive(_get(x_min,y_max,z0))) {
        return true;
    }

    return false;
}

inline bool on_solid_ground(float box_r, float x, float y, float z)
{
    int x_min = (int)(x - box_r);
    int x_max = (int)(x + box_r);

    int y_min = (int)(y - box_r);
    int y_max = (int)(y + box_r);

    int z0 = (int)(z - GROUND_MARGIN);

    //upper left
    //upper right
    //bottom right
    //bottom left

    //in future, check tile height if collision
    if(isSolid(_get(x_max,y_max,z0))) {
        return true;
    }

    if(isSolid(_get(x_max,y_min,z0))) {
        return true; 
    }

    if(isSolid(_get(x_min,y_min,z0))) {
        return true;
    }

    if(isSolid(_get(x_min,y_max,z0))) {
        return true;
    }

    return false;
}

inline bool can_stand_up_inner(int x_min, int x_max, int y_min, int y_max, float z)
{
    int z0 = (int)(z);

    if (isActive(_get(x_min, y_min, z0)) ||
        isActive(_get(x_max, y_min, z0)) ||
        isActive(_get(x_min, y_max, z0)) ||
        isActive(_get(x_max, y_max, z0))) {
        return false;
    }
    return true;
}

// check if there will be a collision if standing up
inline bool can_stand_up(float box_r, float x, float y, float z, float current_h, float new_h)
{
    if (new_h <= current_h) printf("can_stand_up:: new height <= current height. You're doing it wrong\n");
    int x_min = (int)(x - box_r);
    int x_max = (int)(x + box_r);

    int y_min = (int)(y - box_r);
    int y_max = (int)(y + box_r);    

    new_h += z;

    z += current_h;

    bool yes = true;

    yes = can_stand_up_inner(x_min, x_max, y_min, y_max, z);
    if (yes) {
        do {
            z += 1.0f;
            z = (z > new_h) ? (new_h + 0.001f) : z;
            yes = can_stand_up_inner(x_min, x_max, y_min, y_max, z);
        } while (yes && z <= new_h);
    }
    return yes;
}

// checks for collision along vertical bounding axes of agent box
// stepping every 1 unit
inline bool collision_check5(float box_r, float box_h, float x, float y, float z)
{
    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    int n_z = (int)ceil(box_h);

    //z = ceil(z);

    for (int i=0; i<n_z; i++)
    {
        //i = (((float)i) > box_h) ? (int)box_h : i;
        int zz = (int)z + i;
        //printf("i %d\n", i);
        if(isActive(_get(x_max,y_max,zz) != 0))
        {
            //north, west
            //printf("xmax,ymax,z %d %d %d\n", x_max, y_max, zz);
            return true;
        }

        if(isActive(_get(x_max,y_min,zz) != 0))
        {
            //north, east
            //printf("xmax,ymin,z %d %d %d\n", x_max, y_min, zz);
            return true;
        }

        if(isActive(_get(x_min,y_min,zz) != 0))
        {
            //south, east
            //printf("xmin,ymin,z %d %d %d\n", x_min, y_min, zz);
            return true;
        }

        if(isActive(_get(x_min,y_max,zz) != 0))
        {
            //south, west
            //printf("xmin,ymax,z %d %d %d\n", x_min, y_max, zz);
            return true;
        }        

    }
    return false;
}
inline bool collision_check5_stand_up(float box_r, float box_h, float x, float y, float z)
{
    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    int n_z = (int)ceil(box_h);

    for (int i=0; i<n_z; i++)
    {
        int zz = (int)z + i;
        if (i == n_z-1) zz = (int)(z + box_h);  // difference is this cap
        
        if(isActive(_get(x_max,y_max,zz) != 0))
        {
            //north, west
            return true;
        }

        if(isActive(_get(x_max,y_min,zz) != 0))
        {
            //north, east
            return true;
        }

        if(isActive(_get(x_min,y_min,zz) != 0))
        {
            //south, east
            return true;
        }

        if(isActive(_get(x_min,y_max,zz) != 0))
        {
            //south, west
            return true;
        }        

    }
    return false;
}

inline bool collision_check5_z(float box_r, float box_h, float x, float y, float z)
{
    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    int n_z = (int)ceil(box_h) + 1;   // difference is +1

    for (int i=0; i<n_z; i++)
    {
        int zz = (int)z + i;

        if(isActive(_get(x_max,y_max,zz) != 0))
        {
            //north, west
            return true;
        }

        if(isActive(_get(x_max,y_min,zz) != 0))
        {
            //north, east
            return true;
        }

        if(isActive(_get(x_min,y_min,zz) != 0))
        {
            //south, east
            return true;
        }

        if(isActive(_get(x_min,y_max,zz) != 0))
        {
            //south, west
            return true;
        }        

    }
    return false;
}

// like collision_check5, except uses float better step granularity
inline bool collision_check6(float box_r, float box_h, float x, float y, float z)
{
    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    const float step_size = 0.9f;
    int steps = (int)ceil(box_h/step_size);

    for (int i=0; i<steps; i++)
    {
        int zz = (int)(z + i*step_size);
        //if (i == steps-1) zz = (int)(z + box_h);

        if(isActive(_get(x_max,y_max,zz) != 0))
        {
            //north, west
            return true;
        }

        if(isActive(_get(x_max,y_min,zz) != 0))
        {
            //north, east
            return true;
        }

        if(isActive(_get(x_min,y_min,zz) != 0))
        {
            //south, east
            return true;
        }

        if(isActive(_get(x_min,y_max,zz) != 0))
        {
            //south, west
            return true;
        }            

    }
    return false;
}

inline bool collision_check6_z(float box_r, float box_h, float x, float y, float z)
{
    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    const float step_size = 0.9f;
    int steps = (int)ceil(box_h/step_size);

    for (int i=0; i<steps; i++)
    {
        int zz = (int)(z + i*step_size);
        if (i == steps-1) zz = (int)(z + box_h);

        if(isActive(_get(x_max,y_max,zz) != 0))
        {
            //north, west
            return true;
        }

        if(isActive(_get(x_max,y_min,zz) != 0))
        {
            //north, east
            return true;
        }

        if(isActive(_get(x_min,y_min,zz) != 0))
        {
            //south, east
            return true;
        }

        if(isActive(_get(x_min,y_max,zz) != 0))
        {
            //south, west
            return true;
        }        

    }
    return false;
}

inline bool collision_check6_xy(float box_r, float box_h, float x, float y, float z)
{
    int x_min = x - box_r;
    int x_max = x + box_r;

    int y_min = y - box_r;
    int y_max = y + box_r;

    const float step_size = 0.9f;
    int steps = (int)ceil(box_h/step_size);

    for (int i=0; i<steps; i++)
    {
        int zz = (int)(z + i*step_size);
        //if (i == steps-1) zz = (int)(z + box_h);

        if(isActive(_get(x_max,y_max,zz) != 0))
        {
            //north, west
            return true;
        }

        if(isActive(_get(x_max,y_min,zz) != 0))
        {
            //north, east
            return true;
        }

        if(isActive(_get(x_min,y_min,zz) != 0))
        {
            //south, east
            return true;
        }

        if(isActive(_get(x_min,y_max,zz) != 0))
        {
            //south, west
            return true;
        }        

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

        if(isActive(_get(x_max,y_max,zz) != 0))
        {
            //north, west
            return true;
        }

        if(isActive(_get(x_max,y_min,zz) != 0))
        {
            //north, east
            return true;
        }

        if(isActive(_get(x_min,y_min,zz) != 0))
        {
            //south, east
            return true;
        }

        if(isActive(_get(x_min,y_max,zz) != 0))
        {
            //south, west
            return true;
        }        

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

        if(isActive(_get(x_max,y_max,zz) != 0))
        {
            //north, west
            return true;
        }

        if(isActive(_get(x_max,y_min,zz) != 0))
        {
            //north, east
            return true;
        }

        if(isActive(_get(x_min,y_min,zz) != 0))
        {
            //south, east
            return true;
        }

        if(isActive(_get(x_min,y_max,zz) != 0))
        {
            //south, west
            return true;
        }        

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

        if(isActive(_get(x_max,y_max,zz) != 0))
        {
            //north, west
            return true;
        }

        if(isActive(_get(x_max,y_min,zz) != 0))
        {
            //north, east
            return true;
        }

        if(isActive(_get(x_min,y_min,zz) != 0))
        {
            //south, east
            return true;
        }

        if(isActive(_get(x_min,y_max,zz) != 0))
        {
            //south, west
            return true;
        }        

    }
    return false;
}

inline int _collision_check(int x, int y, int z)
{
    return isActive(_get(x,y,z));
}


