#pragma once

#include <c_lib/entity/state.hpp>
#include <c_lib/items/packets.hpp>

/* Interface and templates for all objects */

/* The hierarchy of an object is:
 *
 * ObjectPolicyInterface [ defines common methods API -- actions, networking, and state accessors ]
 *          |
 *          v
 * ObjectStateLayer< ComponentA, ComponentB, ... ComponentN > [template parameterized with StateComponents. e.g. OwnedComponent implements ownership, OwnedDefault returns absurd values that dont correlate with real data ]
 *          |
 *          v
 * ObjectInterface < StateLayer, CreateMessage, StateMessage > [template parameterized with the StateLayer, and object's creation packets]
 *          |
 *          v
 * FinalObject [e.g. Turret]
 *      --final object can also inherit very specialized behaviour, such as TargetAcquisition.
 *      
 */

/* Abstract interface */

class ObjectPolicyInterface
{
    public:
        virtual ObjectState* state() = 0;
        
        // actions
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

        //state
        virtual int get_owner() = 0;
        virtual void set_owner(int owner) = 0;
        virtual int get_team() = 0;
        virtual void set_team(int team) = 0;
        virtual unsigned int get_team_index() = 0;
        virtual void set_team_index(unsigned int team_index) = 0;
        virtual int take_damage(int dmg) = 0;
        virtual bool is_dead() = 0;
        virtual bool did_die() = 0;
        virtual Vec3 get_position() = 0;
        virtual bool set_position(float x, float y, float z) = 0;
        virtual Vec3 get_momentum() = 0;
        virtual void set_momentum(float x, float y, float z) = 0;
        virtual float get_height() = 0;

    ObjectPolicyInterface() {}
    virtual ~ObjectPolicyInterface()
    {
    }
};

/* ObjectStateLayer :
 * Mixin class layer for state setters/getters
 */

template <class Owner, class Team, class Health, class Spatial>  /* more common behaviour state classes here. very specialized classes can stay out */
class ObjectStateLayer: public ObjectPolicyInterface, public Owner, public Team, public Health, public Spatial
{
    protected:
        ObjectState _state;

    public:
        ObjectState* state() { return &this->_state; }
    
        int get_owner() { return Owner::get_owner(); }
        void set_owner(int owner) { Owner::set_owner(this->state()->type, owner); }

        int get_team() { return Team::get_team(); }
        void set_team(int team) { Team::set_team(team); }
        unsigned int get_team_index() { return Team::get_team_index(); }
        void set_team_index(unsigned int team_index) { Team::set_team_index(team_index); }

        int take_damage(int dmg) { return Health::take_damage(dmg); }
        bool is_dead() { return Health::is_dead(); }
        bool did_die() { return Health::did_die(); }

        Vec3 get_position() { return Spatial::get_position(); }
        bool set_position(float x, float y, float z) { return Spatial::set_position(x,y,z); }
        Vec3 get_momentum() { return Spatial::get_momentum(); }
        void set_momentum(float x, float y, float z) { return Spatial::set_momentum(x,y,z); }
        float get_height() { return Spatial::get_height(); }

    ObjectStateLayer<Owner, Team, Health, Spatial>()
    {
    }

    virtual ~ObjectStateLayer<Owner, Team, Health, Spatial>()
    {
    }
};

/* ObjectInterface
 * template accepts message types
 */
template
<
    class StateLayer,
    class CreateMessage,
    class StateMessage
>
class ObjectInterface: public StateLayer
{
    private:
        void createMessage(CreateMessage* msg)
        {
            ObjectState* state = this->state();
            create_message(msg, state->id, state->type, this->get_position(), this->get_momentum(), this->get_owner(), this->get_team(), this->get_team_index());
        }

        void stateMessage(StateMessage* msg)
        {
            ObjectState* state = this->state();
            state_message(msg, state->id, state->type, this->get_position(), this->get_momentum());
        }
        
    public:

    void sendToClientCreate(int client_id)
    {
        CreateMessage msg;
        this->createMessage(&msg);
        msg.sendToClient(client_id);
    }
    
    void broadcastCreate()
    {
        CreateMessage msg;
        this->createMessage(&msg);
        msg.broadcast();
    }
    
    void sendToClientState(int client_id)
    {
        StateMessage msg;
        this->stateMessage(&msg);
        msg.sendToClient(client_id);
    }
    
    void broadcastState()
    {
        StateMessage msg;
        this->stateMessage(&msg);
        msg.broadcast();
    }

    void broadcastDeath()
    {
        #if DC_SERVER
        ObjectState* state = this->state();
        object_destroy_StoC msg;
        msg.id = state->id;
        msg.type = state->type;
        msg.broadcast();
        #endif
    }

    ObjectInterface<StateLayer, CreateMessage, StateMessage>()
    {}

    virtual ~ObjectInterface<StateLayer, CreateMessage, StateMessage>()
    {
    }
};
