#pragma once

#include <c_lib/entity/state.hpp>

/* Abstract interface */

class ObjectPolicyInterface
{
    protected:
        ObjectState _state;
    public:
        ObjectState* state()
        { return &this->_state; }

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


/* ObjectInterface
 * template accepts message types
 */
template
<
    class CreateMessage,
    class StateMessage
>
class ObjectInterface: public ObjectPolicyInterface
{
    public:

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

    ObjectInterface<CreateMessage, StateMessage>() {}
};
