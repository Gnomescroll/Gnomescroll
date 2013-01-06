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


#define ADVANCED_JUMP 0

//class AgentState agent_tick(const struct AgentControlState& _cs, const struct AgentCollisionBox& box, const class AgentState& as)
//{
    //int a_cs = _cs.cs;
    ////set control state variables
    //bool forward     = a_cs & CS_FORWARD ? 1 :0;
    //bool backwards   = a_cs & CS_BACKWARD ? 1 :0;
    //bool left        = a_cs & CS_LEFT ? 1 :0;
    //bool right       = a_cs & CS_RIGHT ? 1 :0;
    //bool jetpack     = a_cs & CS_JETPACK ? 1 :0;
    //bool jump        = a_cs & CS_JUMP ? 1 :0;
    //bool crouch      = a_cs & CS_CROUCH ? 1 :0;
    ////implemented, but unused
    ///*
    //bool boost       = a_cs & CS_BOOST ? 1 :0;
    //bool misc1       = a_cs & CS_MISC1 ? 1 :0;
    //bool misc2       = a_cs & CS_MISC2 ? 1 :0;
    //bool misc3       = a_cs & CS_MISC3 ? 1 :0;
    //*/

    //const float pi = 3.14159265f;
    //const float tr = 1.0f / 10.0f;    //tick rate
    //const float tr2 = tr*tr;
    //const float AGENT_MASS = 1.0f; //the agents mass, will become a variable dependent on the amount of stuff a player carries
    //float imass = 1.0f/AGENT_MASS;   // inverse mass
    //const float FRICTION = 0.9f;    // coefficient of friction. will be variable based on the surface
    //float AGENT_FORCE = 2.0f;


    //float z_gravity = -9.8f * tr2;
    //#if DC_CLIENT
    //if (!t_map::position_is_loaded(as.x, as.y)) z_gravity = 0.0f;
    //#endif
    //const struct Vec3 gravity = vec3_init(0.0f, 0.0f, z_gravity * AGENT_MASS);

    //float fx = 0.0f;
    //float fy = 0.0f;
    //if (forward)
    //{
        //fx += cosf( _cs.theta * pi);
        //fy += sinf( _cs.theta * pi);
    //}
    //if (backwards)
    //{
        //fx += -cosf( _cs.theta * pi);
        //fy += -sinf( _cs.theta * pi);
    //}
    //if (left)
    //{
        //fx += cosf( _cs.theta * pi + pi/2);
        //fy += sinf( _cs.theta * pi + pi/2);
    //}
    //if (right)
    //{
        //fx += -cosf( _cs.theta * pi + pi/2);
        //fy += -sinf( _cs.theta * pi + pi/2);
    //}

    //const float precision = 0.000001f;
    //// normalize diagonal motion
    //if (fx < -precision || fx > precision || fy < -precision || fy > precision)
    //{
        //float len = 1.0f/sqrtf(fx*fx + fy*fy);
        //fx *= len;
        //fy *= len;
    //}


    ////const struct Vec3 f = as.forward_vector();
    ////const struct Vec3 f = vec3_init_from_angles(_cs.theta, _cs.phi, 0.0f);
    //const struct Vec3 p = as.get_position();
    //const struct Vec3 v = as.get_velocity();

    //struct Vec3 traction = vec3_scalar_mult(vec3_init(fx, fy, 0.0f), AGENT_FORCE);
    ////struct Vec3 traction = vec3_scalar_mult(f, AGENT_FORCE);
    //struct Vec3 friction = vec3_scalar_mult(v, -FRICTION);
    //struct Vec3 force = vec3_add(gravity, vec3_add(traction, friction));

    //struct Vec3 accel = vec3_scalar_mult(force, imass);

    //struct Vec3 velo = vec3_add(v, vec3_scalar_mult(accel, tr));
    //struct Vec3 pos = vec3_add(p, vec3_scalar_mult(velo, tr));

    //class AgentState _as;
    //_as.set_position(pos);
    //_as.set_velocity(velo);

    //return _as;
//}

//takes an agent state and control state and returns new agent state
class AgentState agent_tick(const struct AgentControlState& _cs, const struct AgentCollisionBox& box, class AgentState as)
{
    int a_cs = _cs.cs;
    //set control state variables
    bool forward     = a_cs & CS_FORWARD ? 1 :0;
    bool backwards   = a_cs & CS_BACKWARD ? 1 :0;
    bool left        = a_cs & CS_LEFT ? 1 :0;
    bool right       = a_cs & CS_RIGHT ? 1 :0;
    bool jetpack     = a_cs & CS_JETPACK ? 1 :0;
    bool jump        = a_cs & CS_JUMP ? 1 :0;
    bool crouch      = a_cs & CS_CROUCH ? 1 :0;
    //implemented, but unused
    /*
    bool boost       = a_cs & CS_BOOST ? 1 :0;
    bool misc1       = a_cs & CS_MISC1 ? 1 :0;
    bool misc2       = a_cs & CS_MISC2 ? 1 :0;
    bool misc3       = a_cs & CS_MISC3 ? 1 :0;
    */
    const float tr = 1.0f / 10.0f;    //tick rate
    const float tr2 = tr*tr;
    float speed = AGENT_SPEED * tr;
    float height = box.b_height;
    if (crouch)
    {
        speed = AGENT_SPEED_CROUCHED * tr;
        height = box.c_height;
    }

    float z_gravity = -3.0f * tr2;
    #if DC_CLIENT
    if (!t_map::position_is_loaded(as.x, as.y)) z_gravity = 0.0f;
    #endif

    const float z_jetpack = -z_gravity + tr2;

    #if ADVANCED_JUMP
    const float JUMP_POWINITIAL = 1.0f * 0.17f;
    const float JUMP_POWDEC = 0.2f * 0.24f;
    #else
    const float JUMP_POW = AGENT_JUMP_POWER;
    #endif
    //const float z_bounce = 0.10f;
    //const float z_bounce_v_threshold = 1.5f * tr;

    const float pi = 3.14159265f;
    float solid_z = (float)t_map::get_solid_block_below(as.x, as.y, as.z);
    float dist_from_ground = as.z - solid_z;

    float new_x = as.x + as.vx;
    float new_y = as.y + as.vy;
    float new_z = as.z + as.vz;

    //collision
    bool current_collision = collision_check_final_current(box.box_r, height, as.x,as.y,as.z);
    if (current_collision)
    {
        as.x = new_x;
        as.y = new_y;
        as.z += 0.02f; //nudge factor
        if (as.vz < 0.0f) as.vz = 0.0f;
        return as;
    }

    // Collision Order: as.x,as.y,as.z
    bool collision_x = collision_check_final_xy(box.box_r, height, new_x,as.y,as.z);
    if (collision_x)
    {
        new_x = as.x;
        as.vx = 0.0f;
    }

    bool collision_y = collision_check_final_xy(box.box_r, height, new_x,new_y,as.z);
    if (collision_y)
    {
        new_y = as.y;
        as.vy = 0.0f;
    }

    //top and bottom matter
    bool top = false;
    bool collision_z = collision_check_final_z(box.box_r, height, new_x, new_y, new_z, &top);
    if (collision_z)
    {
        new_z = (int)as.z;  // clamp to the floor
        if (top)
            new_z = (float)floor(as.z) + (float)ceil(height) - height;
        as.vz = 0.0f;
    }

    #if ADVANCED_JUMP
    as.jump_pow = new_jump_pow;
    #endif

    float CS_vx = 0.0f;
    float CS_vy = 0.0f;

    if (forward)
    {
        CS_vx += speed*cosf( _cs.theta * pi);
        CS_vy += speed*sinf( _cs.theta * pi);
    }
    if (backwards)
    {
        CS_vx += -speed*cosf( _cs.theta * pi);
        CS_vy += -speed*sinf( _cs.theta * pi);
    }
    if (left)
    {
        CS_vx += speed*cosf( _cs.theta * pi + pi/2);
        CS_vy += speed*sinf( _cs.theta * pi + pi/2);
    }
    if (right)
    {
        CS_vx += -speed*cosf( _cs.theta * pi + pi/2);
        CS_vy += -speed*sinf( _cs.theta * pi + pi/2);
    }

    const float precision = 0.000001f;
    // normalize diagonal motion
    if (CS_vx < -precision || CS_vx > precision || CS_vy < -precision || CS_vy > precision)
    {
        float len = 1.0f/sqrtf(CS_vx*CS_vx + CS_vy*CS_vy);
        CS_vx *= speed*len;
        CS_vy *= speed*len;
    }

    as.vx = CS_vx;
    as.vy = CS_vy;

    // need distance from ground
    const float max_jetpack_height = 8.0f;
    const float jetpack_velocity_max = z_jetpack * 5;
    if (jetpack)
    {
        if (dist_from_ground < max_jetpack_height)
        {   // cap jetpack velocity
            if (as.vz <= jetpack_velocity_max)
                as.vz += z_jetpack;
        }
        else
        if (dist_from_ground < max_jetpack_height + 0.3f)
            as.vz = -z_gravity;
    }

    if (dist_from_ground < 0.025f)
        as.vz -= (new_z - solid_z);
    else
        as.vz += z_gravity;

    #if ADVANCED_JUMP
    float new_jump_pow = as.jump_pow;
    if (jump)
    {
        as.vz = 0.0f;
        new_jump_pow = JUMP_POWINITIAL;
    }
    if (new_jump_pow >= 0)
    {
        as.vz += new_jump_pow;
        new_jump_pow -= JUMP_POWDEC;
    }
    #else
    if (jump)
    {
        as.vz = 0.0f;
        as.vz += JUMP_POW;
    }
    #endif

    as.x = translate_point(new_x);
    as.y = translate_point(new_y);
    if (new_z < solid_z) new_z = solid_z;
    as.z = new_z;
    as.theta = _cs.theta;
    as.phi = _cs.phi;

    return as;
}

}   // Agents
