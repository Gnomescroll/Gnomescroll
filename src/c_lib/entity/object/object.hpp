#pragma once

#include <entity/constants.hpp>
#include <entity/network/interfaces.hpp>

namespace Components { class Component; }   // forward decl

namespace Entities
{

class Entity
{
    public:
        int n_components;
        class Components::Component** components;

        int id;
        EntityType type;

        tickObject tick;        // for physics
        updateObject update;      // for draw prep

        CreatePacketDelegate* create;
        StatePacketDelegate* state;

    // network
    void sendToClientCreate(ClientID client_id)
    {
        if (this->create != NULL) this->create->sendToClient(this, client_id);
    }
    void broadcastCreate()
    {
        if (this->create != NULL) this->create->broadcast(this);
    }
    void sendToClientState(ClientID client_id)
    {
        if (this->state != NULL) this->state->sendToClient(this, client_id);
    }
    void broadcastState()
    {
        if (this->state != NULL) this->state->broadcast(this);
    }
    void broadcastDeath();

    // components
    void add_component(int slot, class Components::Component* component);
    class Components::Component* get_component(ComponentType type);
    class Components::Component* get_component_interface(ComponentInterfaceType interface);

    void init(int n_components);

    ~Entity();

    explicit Entity(int id) :
        n_components(0), components(NULL),
        id(id), type(OBJECT_NONE),
        tick(NULL), update(NULL),
        create(NULL), state(NULL)
    {}
};

} // Entities
