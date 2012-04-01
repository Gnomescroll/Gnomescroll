#pragma once

#include <c_lib/common/enum_types.hpp>
#include <c_lib/physics/verlet.hpp>
#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/items/constants.hpp>
#include <c_lib/ray_trace/handlers.hpp>

typedef enum
{
    COINS_NOBODY  = 0,
    COINS_ANYONE  = 1,
    COINS_ENEMIES = 2,
    COINS_ALLIES  = 4,
    COINS_OWNER   = 8,
    COINS_ANYONE_WHEN_UNOWNED = 16
} COIN_RULES;

/* Holds constants that are dat-configurable */
class ObjectData
{
    public:
        // spatial
        float height;
        float camera_height;

        // physics
        float damp;
        float mass;
        bool broadcast_state_change;

        // tick lifespan
        int ttl_max;

        // pickup
        bool pickup;
        float pickup_radius;

        // explosion
        bool blow_up_on_death;
        float explosion_radius;
        int explosion_damage;
        
        // coins
        unsigned int cost;
        unsigned int reward;
        int coin_rule;

        // firing
        unsigned int fire_rate_limit;
        float sight_range;
        float accuracy_bias;
        float target_acquisition_probability;
        bool attack_enemies;    // TODO -- use bit mask flags
        bool attack_random;
        bool suicidal;  // can kill owner
        struct Hitscan::AttackerProperties attacker_properties;

        // voxel
        bool frozen_vox;
        
        Object_types type;
    //TODO:
    // fill the struct with object metadata
    // all objects' initialization data will be stored in an array
    // when creating an object, array lookup will occur for data, and fill in
    // avoids some massive switch statements
    // can even be inherited into ObjectState
    // and make all properties class members

    // these members should const/immutable for most purposes
    // but they should be adjustable by 1 thing: the dat loader

    // TODO: copy method, for initializing from dat
    //void load(ObjectData& data) {}

    ObjectData()
    :
    height(1.0f), camera_height(0.0f),
    damp(1.0f), mass(1.0f), broadcast_state_change(false),
    ttl_max(100),
    pickup(false), pickup_radius(1.0f),
    blow_up_on_death(false),
    cost(999999), reward(0), coin_rule(COINS_NOBODY),
    fire_rate_limit(1),
    frozen_vox(false),
    type(OBJ_TYPE_NONE)
    {}
};

// encapsulates all information needed for any object,
// merging ObjectData (dat constants) with extra non-dat state
class ObjectState: public ObjectData
{
    public:
        // general status 
        int id;
        int team;
        unsigned int team_index; // id within team
        int owner;

        // life
        int health;
        bool died;
        bool dead;

        // physics
        VerletParticle* vp;
        Vec3 position;
        float theta, phi;
        
        // tick lifespan
        int ttl;

        // draw/textures/voxel
        float texture_scale;
        int texture_index;
        Voxel_model* vox;
        VoxDat* vox_dat;
        bool init_hitscan;
        bool init_draw;

        // pickup
        bool broadcast_death;
        int picked_up_by;

        // firing
        unsigned int fire_tick;

    unsigned int get_kill_reward(int owner, int team);
    int take_damage(int dmg);

    int get_team()
    {
        return this->team;
    }

    void set_team(int team)
    {
        this->team = team;
    }

    int get_owner()
    {
        return this->owner;
    }

    void set_owner(int owner)
    {
        switch_agent_ownership(this->type, this->owner, owner);
        this->owner = owner;
    }
    
    void create_particle(float x, float y, float z, float mx, float my, float mz)
    {
        if (this->vp == NULL)
            this->vp = new VerletParticle(x,y,z, mx,my,mz, this->mass);
    }

    float camera_z()
    {
        float z;
        if (this->vp != NULL)
            z = this->vp->p.z;
        else
            z = this->position.z;
        return z + this->camera_height;
    }

    // returns true if position was different
    bool set_position(float x, float y, float z);
    Vec3 get_position()
    {
        if (this->vp != NULL)
            return this->vp->p;
        else
            return this->position;
    }

    void set_momentum(float mx, float my, float mz)
    {
        if (this->vp != NULL)
            this->vp->set_momentum(mx,my,mz);
        else
            printf("WARNING: ObjectState::set_momentum() -- object type %d does not use momentum\n", this->type);
    }
    
    Vec3 get_momentum()
    {
        if (this->vp != NULL)
            return this->vp->get_momentum();
        else
            return vec3_init(0,0,0);
    }

    Voxel_model* get_vox()
    {
        return this->vox;
    }

    ObjectState()
    : ObjectData(),
    id(-1), team(0), team_index(TEAM_INDEX_NONE), owner(NO_AGENT),
    health(1), died(false), dead(false),
    vp(NULL), theta(0), phi(0), ttl(0), texture_scale(1.0f), texture_index(0),
    vox(NULL), vox_dat(NULL), init_hitscan(false), init_draw(false),
    broadcast_death(false), picked_up_by(-1), fire_tick(0)
    {
        this->set_position(0,0,0);
    }

    ~ObjectState()
    {
        if (this->vp != NULL)
            delete this->vp;
        if (this->vox != NULL)
            delete this->vox;
    }
};


