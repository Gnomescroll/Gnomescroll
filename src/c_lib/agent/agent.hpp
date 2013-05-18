#pragma once

#include <physics/ray_trace/ray_trace.hpp>
#include <entity/constants.hpp>
#include <common/defines.h>
#include <voxel/voxel_model.hpp>
#include <voxel/vox_dat_init.hpp>
#include <agent/constants.hpp>
#include <agent/agent_status.hpp>
#include <physics/common.hpp>

#if DC_CLIENT
# include <agent/client/agent_event.hpp>
#endif

namespace Agents
{

void force_update_agent_vox(class Agent* a);

class AgentState
{
    public:
        int seq;
        float theta;
        float phi;
        float x,y,z;
        float vx,vy,vz;

    AgentState() :
        seq(-1),
        theta(0), phi(0),
        x(0), y(0), z(0),
        vx(0), vy(0), vz(0)
    {}

    struct Vec3 forward_vector();
    struct Vec3 lateral_vector();   // xy-aligned rotation

    struct Vec3 get_position() const
    {
        return vec3_init(x, y, z);
    }

    struct Vec3 get_velocity() const
    {
        return vec3_init(vx, vy, vz);
    }

    void set_position(const struct Vec3& p)
    {
        this->x = translate_point(p.x);
        this->y = translate_point(p.y);
        this->z = p.z;
    }

    void set_velocity(const struct Vec3& v)
    {
        this->vx = v.x;
        this->vy = v.y;
        this->vz = v.z;
    }

    void print()
    {
        printf("p: %0.2f, %0.2f, %0.2f; v: %0.2f, %0.2f, %0.2f;\n", x,y,z,vx,vy,vz);
    }

    private:

    void _constrain_angles();
};

class Agent
{
    private:
        void print_cs();
        ControlState get_current_control_state();
        class AgentState state_rollback;
        struct ControlState cs[256];
        int snapshot_seq;
        int cs_seq; // <--current counter

        class AgentState s; //state current
        class AgentState state_snapshot;

        #if DC_SERVER
        void get_spawn_point(struct Vec3* spawn);
        #endif

        struct BoundingBox standing_box;
        struct BoundingBox crouching_box;

    public:

        AgentID id;
        ClientID client_id;
        UserID user_id;
        EntityType type;

        class AgentStatus status;
        class Voxels::VoxelModel* vox;

        #if DC_CLIENT
        class AgentEvent event;
        bool initial_teleport;  // record first teleport from server
        #endif

    bool crouched();
    bool crouched(const struct ControlState& cs);
    void tick();

    BoundingBox get_bounding_box()
    {
        if (this->crouched())
            return this->crouching_box;
        else
            return this->standing_box;
    }

    BoundingBox get_bounding_box(const struct ControlState& cs)
    {
        if (this->crouched(cs))
            return this->crouching_box;
        else
            return this->standing_box;
    }

    float get_current_speed()
    {
        if (this->crouched())
            return AGENT_SPEED_CROUCHED;
        else
            return AGENT_SPEED;
    }

    Voxels::VoxDat* get_vox_dat()
    {
        if (this->crouched())
            return &VoxDats::agent_crouched;
        else
            return &VoxDats::agent;
    }

    bool in_sight_of(struct Vec3 source, struct Vec3 *sink);
    bool in_sight_of(struct Vec3 source, struct Vec3 *sink, float failure_rate);

    #if DC_SERVER
    AgentState camera;    // agent's camera state, sent by client
    bool camera_ready;
    AgentState get_camera_state()
    {
        return this->camera;
    }
    //Vec3 get_camera_state_position() { return vec3_init(this->camera.x, this->camera.y, this->camera.z); }
    void set_camera_state(float x, float y, float z, float theta, float phi);

    void spawn_state();
    void spawn_state(struct Vec3 p);
    void movement_event(const AgentState& s0, const AgentState& s1);
    void fell(float dvz);
    #endif

    #if DC_CLIENT
    void handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz);
    bool is_you();
    #endif

    AgentState get_state()
    {
        return this->s;
    }
    void set_position(float x, float y, float z);
    void set_state(float x, float y, float z, float vx, float vy, float vz);
    AgentState get_state_snapshot()
    {
        return this->state_snapshot;
    }
    void set_state_snapshot(float  x, float y, float z, float vx, float vy, float vz);
    void set_angles(float theta, float phi);
    void teleport(float x,float y,float z); //should only be used on server
    void teleport(float x,float y,float z, float vx, float vy, float vz, float theta, float phi); //should only be used on server
    void teleport(struct Vec3 p)
    {
        this->teleport(p.x, p.y, p.z);
    }

    // returns side, as integer. side<0 if failure
    int get_facing_side(int solid_pos[3], int open_pos[3], int side[3], float* distance);
    int get_facing_side(int solid_pos[3], int open_pos[3], float* distance);
    int get_facing_side(int solid_pos[3], int open_pos[3], const float max_distance);

    bool nearest_open_block(const float max_dist, int open_point[3]);

    struct Vec3 forward_vector()
    {
        return this->s.forward_vector();
    }

    struct Vec3 lateral_vector()
    {
        return this->s.lateral_vector();
    }

    struct Vec3 get_position()
    {
        return this->s.get_position();
    }

    struct Vec3 get_velocity()
    {
        return this->s.get_velocity();
    }

    struct Vec3 get_camera_position()
    {
        struct Vec3 p = this->get_position();
        p.z += this->camera_height();
        return p;
    }

    struct Vec3 get_center()
    {
        if (this->vox == NULL || !this->vox->was_updated)
        {   // use approximate center of model
            struct Vec3 p = this->get_position();
            p.z += this->current_height() / 2.0f;
            return p;
        }
        return this->vox->get_part(AGENT_PART_TORSO)->get_center();
    }

    //this is for client
    void handle_control_state(int _seq, int _cs, float _theta, float _phi);

    void revert_to_snapshot();
    void revert_to_rollback();

    //int last_direction_change;
    int last_control_state_update_message;  //acts like ghost for now
    int last_full_state_message;

    void draw();
    void update_model();
    void update_legs();

    bool point_can_cast(float x, float y, float z, float max_dist);  // checks if a point can raycast to some area of the agent box, or if the terrain prevents it

    float camera_height();
    float current_height();
    int current_height_int();
    int get_facing_block_type();
    bool near_base();   // is within the base's spawn radius
    void init_vox();

    float camera_z();
    Vec3 camera_position();

    class Voxels::VoxelVolume* get_arm();
    Vec3 arm_center();
    Vec3 arm_forward();
    Vec3 arm_right();
    Vec3 arm_up();

    explicit Agent(AgentID id); //default constructor

    ~Agent();
};

}   // Agents
