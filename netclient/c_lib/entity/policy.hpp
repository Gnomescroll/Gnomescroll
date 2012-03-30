#pragma once

#include <c_lib/entity/state.hpp>
#include <c_lib/entity/behaviour.hpp>

/* Abstract interface */

class ObjectPolicyInterface
{
    public:
        virtual ObjectState* state() = 0;

        virtual void tick() = 0;
        virtual void draw() = 0;
        virtual void update() = 0;
        virtual void born() = 0;
        virtual void die() = 0;
        
        // network
        virtual void sendToClientCreate(int client_id) = 0;
        virtual void broadcastCreate() = 0;
        virtual void sendToClientState(int client_id) = 0;
        virtual void broadcastState() = 0;

        virtual ~ObjectPolicyInterface() {};
};


/* ObjectPolicy
 *
 * Composes composed functions,
 * abstract interface and state into a single object */

//typedef void (*ObjectCreateMessage)(ObjectState* state, object_create_StoC_model*);
//typedef void (*ObjectStateMessage)(ObjectState* state, object_state_StoC_model*);

template
<
    class Wrapper,
    class TickSuper,
    class DrawSuper,
    class UpdateSuper,
    class BornSuper,
    class DieSuper,
    class CreateMessage,
    class StateMessage
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
        
    ObjectState* state() { return &this->_state; }

    void tick() { TickCombiner<TickSuper, Wrapper>::tick(&this->_state, this->subclass); }
    void draw() { DrawCombiner<DrawSuper, Wrapper>::draw(&this->_state, this->subclass); }
    void update() { DrawCombiner<UpdateSuper, Wrapper>::update(&this->_state, this->subclass); }
    void born() { BornCombiner<BornSuper, Wrapper>::born(&this->_state, this->subclass); }
    void die() { DieCombiner<DieSuper, Wrapper>::die(&this->_state, this->subclass); }

    // network
    void sendToClientCreate(int client_id)
    {
        CreateMessage msg;
        create_message(&this->_state, &msg);
        msg.sendToClient(client_id);
    }
    
    void broadcastCreate()
    {
        CreateMessage msg;
        create_message(&this->_state, &msg);
        msg.broadcast();
    }
    
    void sendToClientState(int client_id)
    {
        StateMessage msg;
        state_message(&this->_state, &msg);
        msg.sendToClient(client_id);
    }
    
    void broadcastState()
    {
        StateMessage msg;
        state_message(&this->_state, &msg);
        msg.broadcast();
    }

    ObjectPolicy<Wrapper, TickSuper, DrawSuper, UpdateSuper, BornSuper, DieSuper, CreateMessage, StateMessage>(Wrapper* subclass)
    {
        this->subclass = subclass;
    }
};
