#include "agent_physics.hpp"

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

bool object_collides_terrain(Vec3 position, float height, float radius)
{
    ASSERT_BOXED_POSITION(position);
    return Agents::collision_check_current(radius, height, position.x,
                                           position.y, position.z);
}

namespace Agents
{

bool agent_collides_terrain(Agent* a)
{
    float h = a->current_height();
    Vec3 p = a->get_position();
    return collision_check_current(a->box.box_r, h, p.x, p.y, p.z);
}

#define GROUND_MARGIN 0.003f
// checks the (agent bottom - margin) at 4 corners of the agent
inline bool on_ground(float box_r, float x, float y, float z)
{
    int x_min = int(translate_point(x - box_r));
    int x_max = int(translate_point(x + box_r));
    int y_min = int(translate_point(y - box_r));
    int y_max = int(translate_point(y + box_r));
    int zz = z - GROUND_MARGIN;
    if (t_map::isSolid(x_max, y_max, zz) ||  // north, west
        t_map::isSolid(x_max, y_min, zz) ||  // north, east
        t_map::isSolid(x_min, y_min, zz) ||  // south, east
        t_map::isSolid(x_min, y_max, zz))    // south, west
    {
        return true;
    }
    return false;
}

inline bool can_stand_up(float box_r, float box_h, float x, float y, float z)
{
    int x_min = int(translate_point(x - box_r));
    int x_max = int(translate_point(x + box_r));
    int y_min = int(translate_point(y - box_r));
    int y_max = int(translate_point(y + box_r));
    int n_z = ceilf(box_h);
    for (int i=0; i<n_z; i++)
    {
        int zz = int(z) + i;
        if (i == n_z-1)
            zz = z + box_h;
        if (t_map::isSolid(x_max, y_max, zz) ||  // north, west
            t_map::isSolid(x_max, y_min, zz) ||  // north, east
            t_map::isSolid(x_min, y_max, zz) ||  // south, east
            t_map::isSolid(x_min, y_min, zz))    // south, west
        {
            return true;
        }
    }
    return false;
}

#define TOP_MARGIN 0.01f

inline bool collision_check_current(float box_r, float box_h, float x, float y, float z)
{
    int x_min = int(translate_point(x - box_r));
    int x_max = int(translate_point(x + box_r));
    int y_min = int(translate_point(y - box_r));
    int y_max = int(translate_point(y + box_r));
    for (int i=z; i <= int(z + box_h - TOP_MARGIN*2.0f); i++)
    {
        if (t_map::isSolid(x_max, y_max, i) ||  // north, west
            t_map::isSolid(x_max, y_min, i) ||  // north, east
            t_map::isSolid(x_min, y_max, i) ||  // south, west
            t_map::isSolid(x_min, y_min, i))    // south, east
        {
            return true;
        }
    }
    return false;
}

inline bool collision_check_xy(float box_r, float box_h, float x, float y, float z)
{
    int x_min = int(translate_point(x - box_r));
    int x_max = int(translate_point(x + box_r));
    int y_min = int(translate_point(y - box_r));
    int y_max = int(translate_point(y + box_r));
    const int upper = int(z + box_h - TOP_MARGIN);
    for (int i=z; i <= upper; i++)
    {
        if (t_map::isSolid(x_max, y_max, i) ||  // north, west
            t_map::isSolid(x_max, y_min, i) ||  // north, east
            t_map::isSolid(x_min, y_max, i) ||  // south, west
            t_map::isSolid(x_min, y_min, i))    // south, east
        {
            return true;
        }
    }
    return false;
}

#undef TOP_MARGIN

inline bool collision_check_z(float box_r, float box_h, float x, float y, float z, bool* top)
{
    int x_min = int(translate_point(x - box_r));
    int x_max = int(translate_point(x + box_r));
    int y_min = int(translate_point(y - box_r));
    int y_max = int(translate_point(y + box_r));
    *top = false;
    const int upper = int(z + box_h);
    for (int i=z, j=0; i <= upper; i++, j++)
    {
        if (t_map::isSolid(x_max, y_max, i) ||  // north, west
            t_map::isSolid(x_max, y_min, i) ||  // north, east
            t_map::isSolid(x_min, y_max, i) ||  // south, west
            t_map::isSolid(x_min, y_min, i))    // south, east
        {
            if (i == upper) *top = true;
            return true;
        }
    }
    return false;
}

inline int clamp_to_ground(float box_r, float x, float y, float z)
{
    int x_min = int(translate_point(x - box_r));
    int x_max = int(translate_point(x + box_r));
    int y_min = int(translate_point(y - box_r));
    int y_max = int(translate_point(y + box_r));
    int z0 = t_map::get_solid_block_below(x_min, y_min, int(z));
    int z1 = t_map::get_solid_block_below(x_min, y_max, int(z));
    int z2 = t_map::get_solid_block_below(x_max, y_min, int(z));
    int z3 = t_map::get_solid_block_below(x_max, y_max, int(z));
    return GS_MAX(GS_MAX(z0, z1), GS_MAX(z2, z3)) + 1;
}

#define ADVANCED_JUMP 0

//class AgentState agent_tick(const struct AgentControlState& _cs,
//                            const struct AgentCollisionBox& box,
//                            const class AgentState& as)
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


    //float gravity = -9.8f * tr2;
    //#if DC_CLIENT
    //if (!t_map::position_is_loaded(as.x, as.y)) gravity = 0.0f;
    //#endif
    //const struct Vec3 gravity = vec3_init(0.0f, 0.0f, gravity * AGENT_MASS);

    //float fx = 0.0f;
    //float fy = 0.0f;
    //if (forward)
    //{
        //fx += cosf(_cs.theta * pi);
        //fy += sinf(_cs.theta * pi);
    //}
    //if (backwards)
    //{
        //fx += -cosf(_cs.theta * pi);
        //fy += -sinf(_cs.theta * pi);
    //}
    //if (left)
    //{
        //fx += cosf(_cs.theta * pi + pi/2);
        //fy += sinf(_cs.theta * pi + pi/2);
    //}
    //if (right)
    //{
        //fx += -cosf(_cs.theta * pi + pi/2);
        //fy += -sinf(_cs.theta * pi + pi/2);
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
class AgentState agent_tick(const struct AgentControlState& _cs,
                             const struct AgentCollisionBox& box,
                             class AgentState as)
{
    int a_cs = _cs.cs;
    //set control state variables
    bool forward   = a_cs & CS_FORWARD  ? 1 : 0;
    bool backwards = a_cs & CS_BACKWARD ? 1 : 0;
    bool left      = a_cs & CS_LEFT     ? 1 : 0;
    bool right     = a_cs & CS_RIGHT    ? 1 : 0;
    bool jetpack   = a_cs & CS_JETPACK  ? 1 : 0;
    bool jump      = a_cs & CS_JUMP     ? 1 : 0;
    bool crouch    = a_cs & CS_CROUCH   ? 1 : 0;
    //implemented, but unused
    /*
    bool boost       = a_cs & CS_BOOST ? 1 : 0;
    bool misc1       = a_cs & CS_MISC1 ? 1 : 0;
    bool misc2       = a_cs & CS_MISC2 ? 1 : 0;
    bool misc3       = a_cs & CS_MISC3 ? 1 : 0;
    */

    float height = box.height;
    float speed = AGENT_SPEED * tr;
    if (crouch)
    {
        speed = AGENT_SPEED_CROUCHED * tr;
        height = box.crouch_height;
    }

    float gravity = AGENT_GRAVITY;
    #if DC_CLIENT
    if (!t_map::position_is_loaded(as.x, as.y)) gravity = 0.0f;
    #endif

    #if ADVANCED_JUMP
    const float JUMP_POWINITIAL = 1.0f * 0.17f;
    const float JUMP_POWDEC = 0.2f * 0.24f;
    #else
    const float JUMP_POW = AGENT_JUMP_POWER;
    #endif
    //const float z_bounce = 0.10f;
    //const float z_bounce_v_threshold = 1.5f * tr;

    const float pi = 3.14159265f;
    bool current_collision = collision_check_current(box.box_r, height,
                                                     as.x, as.y, as.z);
    if (current_collision)
    {
        as.z += 0.02f; //nudge factor
        as.vx = 0.0f;
        as.vy = 0.0f;
        as.vz = GS_MAX(as.vz, 0.0f);
        return as;
    }

    float new_x = translate_point(as.x + as.vx);
    float new_y = translate_point(as.y + as.vy);
    float new_z = as.z + as.vz;

    bool collision_x = collision_check_xy(box.box_r, height, new_x, as.y, as.z);
    if (collision_x) new_x = as.x;

    bool collision_y = collision_check_xy(box.box_r, height, new_x, new_y, as.z);
    if (collision_y) new_y = as.y;

    float solid_z = clamp_to_ground(box.box_r, new_x, new_y, as.z);

    bool top = false;
    bool collision_z = collision_check_z(box.box_r, height, new_x, new_y, new_z, &top);
    if (collision_z)
    {   // the "as.vz > 0" check catches erroneous top collisions that occur when
        // falling too fast on top of certain topologies. "top" is really meant to
        // catch upward collisions. NOTE: if vertical velocity ever gets high for
        // some reason (maybe we have a launcher block...), then we probably need
        // to check as.vz < 0 in the else, or something like that.
        if (top && as.vz > 0)
            new_z = floorf(as.z) + ceilf(height) - height;
        else
            new_z = solid_z;
        as.vz = 0.0f;
    }

    new_z = GS_MAX(new_z, solid_z);
    // sometimes we fell down right to the surface edge -- however
    // this is not yet a collision. it will be a collision on the next tick,
    // but there will be no change in z, which is required to distinguish falling
    if (new_z == solid_z) as.vz = 0.0f;
    float dist_from_ground = new_z - solid_z;

    #if ADVANCED_JUMP
    as.jump_pow = new_jump_pow;
    #endif

    float cs_vx = 0.0f;
    float cs_vy = 0.0f;

    if (forward)
    {
        cs_vx += speed*cosf(_cs.theta * pi);
        cs_vy += speed*sinf(_cs.theta * pi);
    }
    if (backwards)
    {
        cs_vx += -speed*cosf(_cs.theta * pi);
        cs_vy += -speed*sinf(_cs.theta * pi);
    }
    if (left)
    {
        cs_vx += speed*cosf(pi * (_cs.theta + 0.5f));
        cs_vy += speed*sinf(pi * (_cs.theta + 0.5f));
    }
    if (right)
    {
        cs_vx += -speed*cosf(pi * (_cs.theta + 0.5f));
        cs_vy += -speed*sinf(pi * (_cs.theta + 0.5f));
    }

    const float precision = 0.000001f;
    if (cs_vx < -precision || cs_vx > precision ||
        cs_vy < -precision || cs_vy > precision)
    {   // normalize diagonal motion
        float len = 1.0f/sqrtf(cs_vx*cs_vx + cs_vy*cs_vy);
        cs_vx *= speed*len;
        cs_vy *= speed*len;
    }

    as.vx = cs_vx;
    as.vy = cs_vy;

    if (jetpack)
    {
        if (dist_from_ground < JETPACK_MAX_HEIGHT)
        {   // cap jetpack velocity
            if (as.vz <= JETPACK_MAX_VELOCITY)
                as.vz += JETPACK_VELOCITY;
        }
        else
        if (dist_from_ground < JETPACK_MAX_HEIGHT + GROUND_MARGIN)
            as.vz = -gravity;
    }

    as.vz += gravity;

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
    as.z = new_z;
    as.theta = _cs.theta;
    as.phi = _cs.phi;
    // cap the fall rate so as not to be able to move so fast as to blip through terrain
    // if we have high lateral or upward velocities we will need to cap those similarly
    // lateral velocities would have to be capped to 2*box_r.  The alternative
    // requires doing collision detection at interpolated intervals
    as.vz = GS_MAX(as.vz, -(height + 0.99f));
    //as.vz = GS_MIN(as.vz, height + 0.99f);
    return as;
}

#undef GROUND_MARGIN


}   // Agents
