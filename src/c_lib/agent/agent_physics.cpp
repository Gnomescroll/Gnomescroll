#include "agent_physics.hpp"

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

#define TOP_MARGIN 0.01f

const float XY_VELOCITY_INTERPOLATION = 0.275f;

inline bool collides_with_terrain(float radius, float box_h, float x, float y, float z)
{
    int x_min = int(translate_point(x - radius));
    int x_max = int(translate_point(x + radius));
    int y_min = int(translate_point(y - radius));
    int y_max = int(translate_point(y + radius));
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

bool object_collides_terrain(Vec3 position, float height, float radius)
{
    GS_ASSERT(is_boxed_position(position));
    return collides_with_terrain(radius, height, position.x, position.y, position.z);
}

inline bool collides_with_terrain_xy(float radius, float box_h, float x, float y, float z)
{
    int x_min = int(translate_point(x - radius));
    int x_max = int(translate_point(x + radius));
    int y_min = int(translate_point(y - radius));
    int y_max = int(translate_point(y + radius));
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

inline bool collision_check_z(float radius, float box_h, float x, float y, float z, bool* top)
{
    int x_min = int(translate_point(x - radius));
    int x_max = int(translate_point(x + radius));
    int y_min = int(translate_point(y - radius));
    int y_max = int(translate_point(y + radius));
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

inline int clamp_to_ground(float radius, float x, float y, float z)
{
    int x_min = int(translate_point(x - radius));
    int x_max = int(translate_point(x + radius));
    int y_min = int(translate_point(y - radius));
    int y_max = int(translate_point(y + radius));
    int z0 = t_map::get_solid_block_below(x_min, y_min, int(z));
    int z1 = t_map::get_solid_block_below(x_min, y_max, int(z));
    int z2 = t_map::get_solid_block_below(x_max, y_min, int(z));
    int z3 = t_map::get_solid_block_below(x_max, y_max, int(z));
    return GS_MAX(GS_MAX(z0, z1), GS_MAX(z2, z3)) + 1;
}

bool collides_with_agents(const BoundingBox& box, const Vec3& position, Agents::Agent*& agent)
{
    using Agents::agent_list;
    using Agents::Agent;
    for (size_t i=0; i<agent_list->max; i++)
    {
        Agent* a = &agent_list->objects[i];
        if (a->id == agent_list->null_id) continue;
        Vec3 p = quadrant_translate_position(position, a->get_center());
        if (bounding_box_intersects(p, a->get_bounding_box(), position, box))
        {
            agent = a;
            return true;
        }
    }
    return false;
}

bool move_with_terrain_collision(const BoundingBox& box, Vec3& position,
                          Vec3& velocity, float& ground_distance, bool agents)
{   // returns false if it collided before moving
    // Note: ground_distance is the difference between the surface z point below
    // the old position and the new position's z point. its weird but its
    // needed to do the jump/jetpack stuff without bugs
    bool current_collision = collides_with_terrain(box.radius, box.height,
                                                   position.x, position.y, position.z);
    if (current_collision)
    {
        position.z += 0.02f; //nudge factor
        velocity.x = 0.0f;
        velocity.y = 0.0f;
        velocity.z = GS_MAX(velocity.z, 0.0f);
        return false;
    }

    Vec3 p = translate_position(vec3_add(position, velocity));

    Agents::Agent* agent = NULL;

    bool collision_x = collides_with_terrain_xy(box.radius, box.height, p.x, position.y, position.z);
    if (!collision_x && agents)
        collides_with_agents(box, vec3_init(p.x, position.y, position.z), agent);
    if (collision_x) p.x = position.x;

    bool collision_y = collides_with_terrain_xy(box.radius, box.height, p.x, p.y, position.z);
    if (!collision_y && agents)
        collides_with_agents(box, vec3_init(p.x, p.y, position.z), agent);
    if (collision_y) p.y = position.y;

    if (agent != NULL)
    {   // push away from the agent
        Vec3 push = vec3_sub(p, agent->get_position());
        push.z = 0.0f;
        if (vec3_length_squared(push) == 0.0f)
            push = vec3_init(randf(), randf(), 0);
        push = vec3_normalize(push);
        const float NUDGE = 0.13f;
        push = vec3_scalar_mult(push, NUDGE);
        velocity = push;
    }

    float solid_z = clamp_to_ground(box.radius, p.x, p.y, position.z);

    bool top = false;
    bool collision_z = collision_check_z(box.radius, box.height, p.x, p.y, p.z, &top);
    if (collision_z)
    {   // the "velocity.z > 0" check catches erroneous top collisions that occur when
        // falling too fast on top of certain topologies. "top" is really meant to
        // catch upward collisions. NOTE: if vertical velocity ever gets high for
        // some reason (maybe we have a launcher block...), then we probably need
        // to check velocity.z < 0 in the else, or something like that.
        if (top && velocity.z > 0)
            p.z = floorf(position.z) + ceilf(box.height) - box.height;
        else
            p.z = solid_z;
        velocity.z = 0.0f;
    }

    p.z = GS_MAX(p.z, solid_z);
    // sometimes we fell down right to the surface edge -- however
    // this is not yet a collision. it will be a collision on the next tick,
    // but there will be no change in z, which is required to distinguish falling
    if (p.z == solid_z) velocity.z = 0.0f;
    position = translate_position(p);
    ground_distance = position.z - solid_z;
    return true;
}

#define GROUND_MARGIN 0.003f
// checks the (agent bottom - margin) at 4 corners of the agent
inline bool on_ground(float radius, const struct Vec3& p)
{
    int x_min = int(translate_point(p.x - radius));
    int x_max = int(translate_point(p.x + radius));
    int y_min = int(translate_point(p.y - radius));
    int y_max = int(translate_point(p.y + radius));
    int zz = p.z - GROUND_MARGIN;
    return (t_map::isSolid(x_max, y_max, zz) ||  // north, west
            t_map::isSolid(x_max, y_min, zz) ||  // north, east
            t_map::isSolid(x_min, y_min, zz) ||  // south, east
            t_map::isSolid(x_min, y_max, zz));    // south, west
}

inline bool can_stand_up(float radius, float box_h, float x, float y, float z)
{
    int x_min = int(translate_point(x - radius));
    int x_max = int(translate_point(x + radius));
    int y_min = int(translate_point(y - radius));
    int y_max = int(translate_point(y + radius));
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

void apply_control_state(const ControlState& cs, float speed, float jump_force,
                         Vec3& position, Vec3& velocity, float ground_distance)
{
    // unpack control state variables
    bool forward   = cs.cs & CS_FORWARD;
    bool backwards = cs.cs & CS_BACKWARD;
    bool left      = cs.cs & CS_LEFT;
    bool right     = cs.cs & CS_RIGHT;
    bool jetpack   = cs.cs & CS_JETPACK;
    bool jump      = cs.cs & CS_JUMP;
    //bool crouch    = cs.cs & CS_CROUCH;
    /* available, but unused
    bool boost     = cs.cs & CS_BOOST;
    bool misc1     = cs.cs & CS_MISC1;
    bool misc2     = cs.cs & CS_MISC2;
    bool misc3     = cs.cs & CS_MISC3;
    */

    float gravity = AGENT_GRAVITY;
    #if DC_CLIENT
    if (!t_map::position_is_loaded(position.x, position.y))
        gravity = 0.0f;
    #endif
    #if ADVANCED_JUMP
    const float JUMP_POWINITIAL = 1.0f * 0.17f;
    const float JUMP_POWDEC = 0.2f * 0.24f;
    #endif

    speed *= PHYSICS_TICK_RATE;

    const float pi = 3.14159265f;
    float cs_vx = 0.0f;
    float cs_vy = 0.0f;

    if (forward)
    {
        cs_vx += speed*cosf(cs.theta * pi);
        cs_vy += speed*sinf(cs.theta * pi);
    }
    if (backwards)
    {
        cs_vx += -speed*cosf(cs.theta * pi);
        cs_vy += -speed*sinf(cs.theta * pi);
    }
    if (left)
    {
        cs_vx += speed*cosf(pi * (cs.theta + 0.5f));
        cs_vy += speed*sinf(pi * (cs.theta + 0.5f));
    }
    if (right)
    {
        cs_vx += -speed*cosf(pi * (cs.theta + 0.5f));
        cs_vy += -speed*sinf(pi * (cs.theta + 0.5f));
    }

    const float precision = 0.00005f;
    if (fabsf(cs_vx) < precision)
        cs_vx = 0.0f;
    if (fabsf(cs_vy) < precision)
        cs_vy = 0.0f;

    //if (cs_vx < -precision || cs_vx > precision ||
        //cs_vy < -precision || cs_vy > precision)
    if (cs_vx || cs_vy)
    {   // normalize diagonal motion
        float len = 1.0f/sqrtf(cs_vx*cs_vx + cs_vy*cs_vy);
        cs_vx *= speed * len;
        cs_vy *= speed * len;
    }

    //velocity.x = cs_vx;
    //velocity.y = cs_vy;
    velocity.x = interpolate(velocity.x, cs_vx, XY_VELOCITY_INTERPOLATION);
    velocity.y = interpolate(velocity.y, cs_vy, XY_VELOCITY_INTERPOLATION);

    if (jetpack)
    {
        if (ground_distance < JETPACK_MAX_HEIGHT)
        {   // cap jetpack velocity
            if (velocity.z <= JETPACK_MAX_VELOCITY)
                velocity.z += JETPACK_VELOCITY;
        }
        else
        if (ground_distance < JETPACK_MAX_HEIGHT + GROUND_MARGIN)
            velocity.z = -gravity;
    }

    velocity.z += gravity;

    #if ADVANCED_JUMP
    float new_jump_pow = as.jump_pow;
    if (jump)
    {
        velocity.z = 0.0f;
        new_jump_pow = JUMP_POWINITIAL;
    }
    if (new_jump_pow >= 0)
    {
        velocity.z += new_jump_pow;
        new_jump_pow -= JUMP_POWDEC;
    }
    #else
    if (jump)
    {
        velocity.z = 0.0f;
        velocity.z += jump_force;
    }
    #endif
}

namespace Agents
{

bool agent_collides_terrain(Agent* a)
{
    float h = a->current_height();
    Vec3 p = a->get_position();
    return collides_with_terrain(a->get_bounding_box().radius, h, p.x, p.y, p.z);
}

#define ADVANCED_JUMP 0

//class AgentState agent_tick(const struct ControlState& _cs,
//                            const struct BoundingBox& box,
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
    //const float PHYSICS_TICK_RATE = 1.0f / 10.0f;    //tick rate
    //const float PHYSICS_TICK_RATE_SQ = PHYSICS_TICK_RATE*PHYSICS_TICK_RATE;
    //const float AGENT_MASS = 1.0f; //the agents mass, will become a variable dependent on the amount of stuff a player carries
    //float imass = 1.0f/AGENT_MASS;   // inverse mass
    //const float FRICTION = 0.9f;    // coefficient of friction. will be variable based on the surface
    //float AGENT_FORCE = 2.0f;


    //float gravity = -9.8f * PHYSICS_TICK_RATE_SQ;
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

    //struct Vec3 velo = vec3_add(v, vec3_scalar_mult(accel, PHYSICS_TICK_RATE));
    //struct Vec3 pos = vec3_add(p, vec3_scalar_mult(velo, PHYSICS_TICK_RATE));

    //class AgentState _as;
    //_as.set_position(pos);
    //_as.set_velocity(velo);

    //return _as;
//}

//takes an agent state and control state and returns new agent state
class AgentState agent_tick(const struct ControlState& cs,
                            const BoundingBox& box, class AgentState as)
{
    Vec3 position = as.get_position();
    Vec3 velocity = as.get_velocity();
    float ground_distance = 0.0f;
    bool passed_through = move_with_terrain_collision(box, position, velocity,
                                              ground_distance);
    if (!passed_through)
    {
        as.set_position(position);
        as.set_velocity(velocity);
        return as;
    }

    #if ADVANCED_JUMP
    as.jump_pow = new_jump_pow;
    #endif

    float speed = AGENT_SPEED;
    if (cs.cs & CS_CROUCH)
        speed = AGENT_SPEED_CROUCHED;

    apply_control_state(cs, speed, AGENT_JUMP_POWER, position, velocity,
                        ground_distance);

    as.theta = cs.theta;
    as.phi = cs.phi;
    // cap the fall rate so as not to be able to move so fast as to blip through terrain
    // if we have high lateral or upward velocities we will need to cap those similarly
    // lateral velocities would have to be capped to 2*radius.  The alternative
    // requires doing collision detection at interpolated intervals
    velocity.z = GS_MAX(velocity.z, -(box.height + 0.99f));
    //as.vz = GS_MIN(as.vz, height + 0.99f);
    as.set_position(position);
    as.set_velocity(velocity);
    return as;
}

#undef GROUND_MARGIN

}   // Agents
