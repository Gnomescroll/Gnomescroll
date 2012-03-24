#pragma once

#include <c_lib/common/enum_types.hpp>
#include <c_lib/template/object_list.hpp>

#include <c_lib/items/pickup.hpp>
#include <c_lib/items/refills.hpp>

namespace ItemDrops
{

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

template <class Super, typename State>
class DrawCombiner: public Super
{
    public:
        void draw(State* state)
        {
            Super::draw(state);
        }
};

template <typename State>
class DrawAnchor
{
    public:
        void draw(State* state) 
        { 
        }
};

template <class Super, typename State>
class TickCombiner: public Super
{
    public:
        void tick(State* state)
        {
            Super::tick(state);
        }
};

template <typename State>
class TickAnchor
{
    public:
        void tick(State* state) 
        { 
        }
};

template <class Super, typename State>
class DieCombiner: public Super
{
    public:
        void die(State* state)
        {
            Super::die(state);
        }
};

template <typename State>
class DieAnchor
{
    public:
        void die(State* state) 
        { 
        }
};

template <class Super, typename State>
class DieBlowup: public Super
{
    public:
        void die(State* state)
        {
            Super::die(state);
        }
};

template <class Super, typename State>
class DiePickup: public Super
{
    public:
        void die(State* state);
};

#define NoDie(STATE) DieAnchor<STATE>
#define NoTick(STATE) TickAnchor<STATE>
#define NoDraw(STATE) DrawAnchor<STATE>

class ObjectPolicyInterface
{
    public:
        virtual void die() = 0;
        virtual void tick() = 0;
        virtual void draw() = 0;
        virtual ObjectState* state() = 0;
};

template
<
    class Wrapper,
    class DieSuper,
    class TickSuper,
    class DrawSuper
>
class ObjectPolicy:
public DieCombiner<DieSuper, ObjectStateTemplate<Wrapper> >,
public TickCombiner<TickSuper, ObjectStateTemplate<Wrapper> >,
public DrawCombiner<DrawSuper, ObjectStateTemplate<Wrapper> >,
public ObjectPolicyInterface
{
    public:
        ObjectStateTemplate<Wrapper> _state;
    
    void die() { DieCombiner<DieSuper, ObjectStateTemplate<Wrapper> >::die(&this->_state); }
    void tick() { TickCombiner<TickSuper, ObjectStateTemplate<Wrapper> >::tick(&this->_state); }
    void draw() { DrawCombiner<DrawSuper, ObjectStateTemplate<Wrapper> >::draw(&this->_state); }
    ObjectState* state() { return &this->_state; }

    ObjectPolicy<Wrapper, DieSuper, TickSuper, DrawSuper>(Wrapper* wrapper)
    {
        _state.object = wrapper;
    }
};


template <class Super, typename State>
class TickParticle: public Super
{
    public:
    void tick(State* state)
    {
        Verlet::bounce(state->vp, state->damp);
        Super::tick(state);
    }
};

template <class Super, typename State>
class TickTTL: public Super
{
    public:
    void tick(State* state)
    {
        state->ttl++;
        Super::tick(state);
    }
};

template <class Super, typename State>
class TickPickup: public Super
{
    public:
    void tick(State* state);
};

template <class Super, typename State>
class DrawBillboardSprite: public Super
{
    public:
    void draw(State* state);
};

class GameObject_list: public Object_list<ObjectPolicyInterface, GAME_OBJECTS_MAX>
{
    public:
        const char* name() { return "GameObject"; }
        
        void tick();
        void draw();
        ObjectPolicyInterface* create(float x, float y, float z, float mx, float my, float mz, Object_types type);
        void destroy(int id);
        
        GameObject_list() { print(); }
};

// TODO: restructure the list creation to adapt based on type

} // ItemDrops
