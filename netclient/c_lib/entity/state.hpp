#pragma once

#include <c_lib/common/enum_types.hpp>

/* Holds constants that are dat-configurable */
class ObjectData
{
    public:

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
};

// encapsulates all information needed for any object,
// merging ObjectData (dat constants) with extra non-dat state
class ObjectState: public ObjectData
{
    public:
        int id;

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

// encapsulates all information needed for any object
template <class Subclass>
class ObjectStateTemplate: public ObjectState
{
    public:
    Subclass* object;
    ObjectStateTemplate<Subclass>() {}
};
