#pragma once

#include <c_lib/common/enum_types.hpp>

struct ObjectData
{
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

// encapsulates all information needed for any object
class ObjectState
{
    public:
    int id;
    Object_types type;
    bool blow_up_on_death;
    bool pickup;
    float pickup_radius;
    int ttl;
    int ttl_max;
    float damp;
    VerletParticle* vp;
    float mass;
    float texture_scale;
    int texture_index;
    bool broadcast_death;
    int picked_up_by;
    
    ObjectState()
    : id(-1), type(OBJ_TYPE_NONE), blow_up_on_death(false), pickup(false),
    pickup_radius(1.0f), ttl(0), ttl_max(1), damp(1.0f), vp(NULL),
    mass(1.0f), texture_scale(1.0f), texture_index(0),
    broadcast_death(false), picked_up_by(-1)
    {}

    ~ObjectState()
    {
        if (this->vp != NULL)
            delete this->vp;
    }

    // TODO: setters
};

// encapsulates all information needed for any object
template <class Subclass>
class ObjectStateTemplate: public ObjectState
{
    public:
    Subclass* object;
    ObjectStateTemplate<Subclass>() {}
};
