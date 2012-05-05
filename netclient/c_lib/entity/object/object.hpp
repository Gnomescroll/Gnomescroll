#pragma once

#include <entity/constants.hpp>
#include <entity/component/component.hpp>
#include <entity/network/interfaces.hpp>

////forward declaration
//namespace Components
//{
    //class Component;
//}

namespace Objects
{

using Components::Component;

class Object
{
    public:
        int n_components;
        class Component** components;

        int id;
        ObjectType type;

        void (*tick)(Object*);      // for physics
        void (*update)(Object*);    // for draw prep

        CreatePacketDelegate* create;
        StatePacketDelegate* state;

        // network
        void sendToClientCreate(int client_id)
        {
            if (this->create != NULL) this->create->sendToClient(this, client_id);
        }
        void broadcastCreate()
        {
            if (this->create != NULL) this->create->broadcast(this);
        }
        void sendToClientState(int client_id)
        {
            if (this->state != NULL) this->state->sendToClient(this, client_id);
        }
        void broadcastState()
        {
            if (this->state != NULL) this->state->broadcast(this);
        }
        void broadcastDeath();

        // components
        void add_component(int slot, class Component* component);
        class Component* get_component(ComponentType type);
        class Component* get_component_interface(ComponentInterfaceType interface);

        void init(int n_components);

    ~Object();

    explicit Object(int id)
    :   n_components(0), components(NULL),
        id(id), type(OBJECT_NONE),
        tick(NULL), update(NULL),
        create(NULL), state(NULL)
    {}
};


} // Objects
