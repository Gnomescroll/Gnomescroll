/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <entity/constants.hpp>
#include <entity/entity/config.hpp>
#include <entity/network/interfaces.hpp>

namespace Components { class Component; }   // forward decl

namespace Entities
{

class Entity
{
    public:
        int n_components;
        class Components::Component** components;

        EntityID id;
        EntityType type;

    // network
    void sendToClientCreate(ClientID client_id) const
    {
        CreatePacketDelegate* create = get_entity_create_packet_delegate(this->type);
        if (create != NULL) create->sendToClient(this, client_id);
    }

    void broadcastCreate() const
    {
        CreatePacketDelegate* create = get_entity_create_packet_delegate(this->type);
        if (create != NULL) create->broadcast(this);
    }

    void sendToClientState(ClientID client_id) const
    {
        StatePacketDelegate* state = get_entity_state_packet_delegate(this->type);
        if (state != NULL) state->sendToClient(this, client_id);
    }

    void broadcastState() const
    {
        StatePacketDelegate* state = get_entity_state_packet_delegate(this->type);
        if (state != NULL) state->broadcast(this);
    }

    void broadcastDeath();

    // components
    void add_component(int slot, class Components::Component* component);
    class Components::Component* get_component(ComponentType type) const;
    class Components::Component* get_component_interface(ComponentInterfaceType interface) const;

    void init(size_t n_components);
    void release_components();

    void ready();
    void destroy();
    void update();
    void after_update();
    void before_tick();
    void tick();
    void after_tick();

    ~Entity();

    explicit Entity(EntityID id) :
        n_components(0), components(NULL),
        id(id), type(NULL_ENTITY_TYPE)
    {}
};

} // Entities
