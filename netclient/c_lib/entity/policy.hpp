#pragma once

#include <c_lib/entity/state.hpp>
#include <c_lib/entity/behaviour.hpp>

//forward decl
class object_create_StoC_model;

/* Abstract interface */

class ObjectPolicyInterface
{
    public:
        virtual void tick() = 0;
        virtual void draw() = 0;
        virtual void update() = 0;
        virtual void born() = 0;
        virtual void die() = 0;
        virtual void create_message(object_create_StoC_model* msg) = 0;
        virtual ObjectState* state() = 0;
        virtual ~ObjectPolicyInterface() {};
};


/* ObjectPolicy
 *
 * Composes composed functions,
 * abstract interface and state into a single object */

typedef void (*ObjectCreateMessage)(ObjectState* state, object_create_StoC_model*);
typedef void (*ObjectStateMessage)(ObjectState* state, object_state_StoC_model*);

template
<
    class Wrapper,
    class TickSuper,
    class DrawSuper,
    class UpdateSuper,
    class BornSuper,
    class DieSuper,
    ObjectCreateMessage CreateMessage,
    ObjectStateMessage StateMessage
>
class ObjectPolicy:
public TickCombiner<TickSuper, Wrapper>,
public DrawCombiner<DrawSuper, Wrapper>,
public DrawCombiner<UpdateSuper, Wrapper>,
public BornCombiner<BornSuper, Wrapper>,
public DieCombiner<DieSuper, Wrapper>,
public ObjectPolicyInterface
{
    public:
        ObjectState _state;
        Wrapper* subclass;
        
    void tick() { TickCombiner<TickSuper, Wrapper>::tick(&this->_state, this->subclass); }
    void draw() { DrawCombiner<DrawSuper, Wrapper>::draw(&this->_state, this->subclass); }
    void update() { DrawCombiner<UpdateSuper, Wrapper>::update(&this->_state, this->subclass); }
    void born() { BornCombiner<BornSuper, Wrapper>::born(&this->_state, this->subclass); }
    void die() { DieCombiner<DieSuper, Wrapper>::die(&this->_state, this->subclass); }
    void create_message(object_create_StoC_model* msg) { CreateMessage(&this->_state, msg); }
    void state_message(object_state_StoC_model* msg) { StateMessage(&this->_state, msg); }
    ObjectState* state() { return &this->_state; }

    ObjectPolicy<Wrapper, TickSuper, DrawSuper, UpdateSuper, BornSuper, DieSuper, CreateMessage, StateMessage>(Wrapper* subclass)
    {
        this->subclass = subclass;
    }
};
