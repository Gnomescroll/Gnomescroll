#pragma once

#include <c_lib/common/enum_types.hpp>
#include <c_lib/physics/verlet.hpp>
#include <c_lib/voxel/voxel_model.hpp>

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
        // camera
        float camera_height;

        // physics
        float damp;
        float mass;

        // tick lifespan
        int ttl_max;

        // pickup
        bool pickup;
        float pickup_radius;

        // explosion
        bool blow_up_on_death;

        //spawning
        unsigned int spawn_radius;

        // coins
        unsigned int cost;
        unsigned int reward;
        int coin_rule;

        // firing
        unsigned int fire_rate_limit;


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
    camera_height(0.0f),
    damp(1.0f), mass(1.0f), ttl_max(100),
    pickup(false), pickup_radius(1.0f),
    blow_up_on_death(false),
    spawn_radius(1),
    cost(999999), reward(0), coin_rule(COINS_NOBODY),
    fire_rate_limit(1),
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
        int team_index; // id within team
        int owner;
        int health;

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

    int get_kill_reward(int owner, int team);
    int take_damage(int dmg);

    int get_team();
    void set_team(int team);
    
    int get_owner();
    void set_owner(int owner);
    
    ObjectState()
    : ObjectData(),
    id(-1), vp(NULL), theta(0), phi(0), ttl(0), texture_scale(1.0f), texture_index(0),
    vox(NULL), vox_dat(NULL), init_hitscan(false), init_draw(false),
    broadcast_death(false), picked_up_by(-1), fire_tick(0)
    {
        this->position.x = 0;
        this->position.y = 0;
        this->position.z = 0;
    }

    ~ObjectState()
    {
        if (this->vp != NULL)
            delete this->vp;
    }

    void create_particle(float x, float y, float z, float mx, float my, float mz)
    {
        if (this->vp == NULL)
            this->vp = new VerletParticle(x,y,z, mx,my,mz, this->mass);
    }

    Vec3 get_position()
    {
        if (this->vp != NULL)
            return this->vp->p;
        else
            return this->position;
    }
};
