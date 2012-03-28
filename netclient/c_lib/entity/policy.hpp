#pragma once

#include <c_lib/entity/state.hpp>
#include <c_lib/entity/behaviour.hpp>

/* Abstract interface */

class ObjectPolicyInterface
{
    public:
        virtual void tick() = 0;
        virtual void draw() = 0;
        virtual void update() = 0;
        virtual void born() = 0;
        virtual void die() = 0;
        virtual ObjectState* state() = 0;
};


/* ObjectPolicy
 *
 * Composes composed functions,
 * abstract interface and state into a single object */

template
<
    class Wrapper,
    class TickSuper,
    class DrawSuper,
    class UpdateSuper,
    class BornSuper,
    class DieSuper
>
class ObjectPolicy:
public TickCombiner<TickSuper, ObjectStateTemplate<Wrapper> >,
public DrawCombiner<DrawSuper, ObjectStateTemplate<Wrapper> >,
public DrawCombiner<UpdateSuper, ObjectStateTemplate<Wrapper> >,
public BornCombiner<BornSuper, ObjectStateTemplate<Wrapper> >,
public DieCombiner<DieSuper, ObjectStateTemplate<Wrapper> >,
public ObjectPolicyInterface
{
    public:
        ObjectStateTemplate<Wrapper> _state;
    
    void tick() { TickCombiner<TickSuper, ObjectStateTemplate<Wrapper> >::tick(&this->_state); }
    void draw() { DrawCombiner<DrawSuper, ObjectStateTemplate<Wrapper> >::draw(&this->_state); }
    void update() { DrawCombiner<UpdateSuper, ObjectStateTemplate<Wrapper> >::update(&this->_state); }
    void born() { BornCombiner<BornSuper, ObjectStateTemplate<Wrapper> >::born(&this->_state); }
    void die() { DieCombiner<DieSuper, ObjectStateTemplate<Wrapper> >::die(&this->_state); }
    ObjectState* state() { return &this->_state; }

    ObjectPolicy<Wrapper, TickSuper, DrawSuper, UpdateSuper, BornSuper, DieSuper>(Wrapper* wrapper)
    {
        _state.object = wrapper;    // pointer to subclass
    }
};
