#pragma once

#include <c_lib/common/enum_types.hpp>

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

        // buying
        unsigned int cost;

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
    damp(1.0f), mass(1.0f), ttl_max(100), pickup(false), pickup_radius(1.0f),
    blow_up_on_death(false), type(OBJ_TYPE_NONE)
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

        // tick lifespan
        int ttl;

        // draw/textures
        float texture_scale;
        int texture_index;

        // pickup
        bool broadcast_death;
        int picked_up_by;

        // firing
        unsigned int fire_tick;

    int take_damage(int dmg);
    int get_coins_for_kill(int owner, int team);

    int get_team();
    void set_team(int team);
    
    int get_owner();
    void set_owner(int owner);
    
    ObjectState()
    : ObjectData(),
    id(-1), vp(NULL), ttl(0), texture_scale(1.0f), texture_index(0),
    broadcast_death(false), picked_up_by(-1)
    {}

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
};
