#pragma once

#include <c_lib/objects/common/interface/state.hpp>
//#include <c_lib/objects/common/interface/interfaces.hpp>

/* Interface and templates for all objects */

/* The hierarchy of an object is:
 *
 * ObjectPolicyInterface [ defines common methods API -- actions, networking, and state accessors ]
 *          |
 *          v
 * ObjectStateLayer< ComponentA, ComponentB, ... ComponentN > [template parameterized with StateComponents. e.g. OwnedComponent implements ownership, OwnedNone returns absurd values that dont correlate with real data ]
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
        virtual void tick() = 0;    // these can be in lists
        virtual void draw() = 0;
        virtual void update() = 0;
        
        virtual void born(int subtype) = 0;    // called individual on objects at birth/death (object type is always known here)
        virtual void die() = 0;
        
        // network
        virtual void sendToClientCreate(int client_id) = 0; // would have to encapsulate elsewhere -- network_list ?
        virtual void broadcastCreate() = 0;
        virtual void sendToClientState(int client_id) = 0;
        virtual void broadcastState() = 0;
        virtual void broadcastDeath() = 0;

        //state
        virtual int get_owner() = 0;    // kind of stuck with these
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
        virtual Vec3 get_angles() = 0;
        virtual bool set_angles(float theta, float phi, float rho) = 0;

    ObjectPolicyInterface() {}
    virtual ~ObjectPolicyInterface()
    {
    }
};

/* Network Delegates */

class CreatePacketDelegate
{
    public:
        virtual void sendToClient(ObjectPolicyInterface* obj, int client_id) = 0;
        virtual void broadcast(ObjectPolicyInterface* obj) = 0;
        virtual ~CreatePacketDelegate(){}
};

class StatePacketDelegate
{
    public:
        virtual void sendToClient(ObjectPolicyInterface* obj, int client_id) = 0;
        virtual void broadcast(ObjectPolicyInterface* obj) = 0;
        virtual ~StatePacketDelegate(){}
};


/* Null Objects */


class CreatePacketNone: public CreatePacketDelegate
{
    public:
        void sendToClient(ObjectPolicyInterface* obj, int client_id) {}
        void broadcast(ObjectPolicyInterface* obj) {}
};

class StatePacketNone: public StatePacketDelegate
{
    public:
        void sendToClient(ObjectPolicyInterface* obj, int client_id) {}
        void broadcast(ObjectPolicyInterface* obj) {}
};


/* ObjectInterface
 * template accepts message types
 */

 //forward decl
class OwnedNone;
class TeamNone;
class HealthNone;
class SpatialNone;

namespace Objects
{
void broadcastDeath(ObjectState* state);

extern CreatePacketNone* create_packet_none;
extern StatePacketNone* state_packet_none;

extern OwnedNone* owned_none;
extern TeamNone* team_none;
extern HealthNone* health_none;
extern SpatialNone* spatial_none;
}

/* ObjectStateLayer :
 * Mixin class layer for state setters/getters
 */

class ObjectStateLayer: public ObjectPolicyInterface
{
    private:
        CreatePacketDelegate* create_packet;
        StatePacketDelegate* state_packet;

    protected:
        ObjectState _state;

        OwnedDelegate* owned;
        TeamDelegate* team;
        HealthDelegate* health;
        SpatialDelegate* spatial;

    public:
        ObjectState* state() { return &this->_state; }

        // Owner
        int get_owner() { return this->owned->get_owner(); }
        void set_owner(int owner) { this->owned->set_owner(this->state()->id, this->state()->type, owner); }

        // Team
        int get_team() { return this->team->get_team(); }
        void set_team(int team) { this->team->set_team(team); }
        unsigned int get_team_index() { return this->team->get_team_index(); }
        void set_team_index(unsigned int team_index) { this->team->set_team_index(team_index); }

        // Health
        int take_damage(int dmg) { return this->health->take_damage(dmg); }
        bool is_dead() { return this->health->is_dead(); }
        bool did_die() { return this->health->did_die(); }

        // Spatial
        Vec3 get_position() { return this->spatial->get_position(); }
        bool set_position(float x, float y, float z) { return this->spatial->set_position(x,y,z); }
        Vec3 get_momentum() { return this->spatial->get_momentum(); }
        void set_momentum(float x, float y, float z) { return this->spatial->set_momentum(x,y,z); }
        float get_height() { return this->spatial->get_height(); }
        Vec3 get_angles() { return this->spatial->get_angles(); }
        bool set_angles(float theta, float phi, float rho) { return this->spatial->set_angles(theta, phi, rho); }

        /* Network API */
        void sendToClientCreate(int client_id) { this->create_packet->sendToClient(this, client_id); }
        void broadcastCreate() { this->create_packet->broadcast(this); }
        void sendToClientState(int client_id) { this->state_packet->sendToClient(this, client_id); }
        void broadcastState() { this->state_packet->broadcast(this); }
        void broadcastDeath() { Objects::broadcastDeath(this->state()); }

    ObjectStateLayer(CreatePacketDelegate* create, StatePacketDelegate* state,
        OwnedDelegate* owned, TeamDelegate* team, HealthDelegate* health, SpatialDelegate* spatial
    ):
    create_packet(create), state_packet(state),
    owned(owned), team(team), health(health), spatial(spatial)
    {
    }
    
    ObjectStateLayer()  // all defaults
    : create_packet(Objects::create_packet_none), state_packet(Objects::state_packet_none),
    owned(Objects::owned_none), team(Objects::team_none), health(Objects::health_none),
    spatial(Objects::spatial_none)
    {
    }

    //ObjectStateLayer(OwnedDelegate* owned, TeamDelegate* team, HealthDelegate* health, SpatialDelegate* spatial)
    //:
    //owned(owned), team(team), health(health), spatial(spatial)
    //{
    //}
    
    //ObjectStateLayer()  // all defaults
    //:
    //owned(Objects::owned_none), team(Objects::team_none), health(Objects::health_none),
    //spatial(Objects::spatial_none)
    //{
    //}

    virtual ~ObjectStateLayer()
    {
    }
};

/*
 * List concepts:
 *
 *  -- Creation/Deletion (die,born)
 *
 *  -- Tick, Draw, Update (one each)
 *
 *  -- Subcomponents, not shared in any way
 *
 *  -- Network interfaces
 *
 * at the far end of this, any special case iteration would have its own list and the objects
 * would need to register with it, along with copies/pointers to the data needed for that
 * iterator
 */

/* Separate Network Layer */
/* Problems:
 *  sending in the data needed for the packet, generically
 *  -- can send in all data ever needed
 *  -- instantiate with the base object (requires base object to be an instance of ObjectPolicyInterface  
 *
 *
 * Add new list, network_list
 * like an property tracking list
 * -- have to be able to pass an object that inherits ObjectNetworkLayer 
 * -- the cast will be from the base object, registering itself with the network list on creation
 * 
 */

//class ObjectNetworkLayer
//{
    //private:
        //ObjectPolicyInterface* object;
        //CreatePacketDelegate* create_packet;
        //StatePacketDelegate* state_packet;

    //public:
        ///* Network API */                               
        //void sendToClientCreate(int client_id) { this->create_packet->sendToClient(this->object, client_id); }
        //void broadcastCreate() { this->create_packet->broadcast(this->object); }
        //void sendToClientState(int client_id) { this->state_packet->sendToClient(this->object, client_id); }
        //void broadcastState() { this->state_packet->broadcast(this->object); }
        //void broadcastDeath() { Objects::broadcastDeath(this->object->state()); }

    //ObjectNetworkLayer(ObjectPolicyInterface* object, CreatePacketDelegate* create, StatePacketDelegate* state)
    //:
    //object(object), create_packet(create), state_packet(state)
    //{
    //}
    
    //ObjectNetworkLayer(ObjectPolicyInterface* object)  // all defaults
    //: object(object), create_packet(Objects::create_packet_none), state_packet(Objects::state_packet_none)
    //{
    //}

    //~ObjectNetworkLayer()
    //{
    //}
//};

/* Placeholders
 *
 *  Use while implementing a new object (so dont need to implement everything at once)
 *  Or if object doesnt need anything
 * */

#define DUMMY_NETWORK_INTERFACE void sendToClientCreate(int client_id){}void broadcastCreate(){}void sendToClientState(int client_id){}void broadcastState(){}void broadcastDeath(){}
#define DUMMY_API_INTERFACE void tick(){}void draw(){}void update(){}void die(){}void born(int subtype){}
