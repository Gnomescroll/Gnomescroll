/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "entity.hpp"

#include <entity/constants.hpp>
#include <entity/network/packets.hpp>
#include <entity/component/_interface.hpp>

namespace Entities
{

void Entity::release_components()
{
    IF_ASSERT(this->components == NULL) return;
    for (int i=0; i<this->n_components; i++)
    {
        if (this->components[i] == NULL) continue;
        this->components[i]->entity = NULL;
        Components::release(this->components[i]);
        this->components[i] = NULL;
    }
}

void Entity::add_component(int slot, class Components::Component* component)
{
    IF_ASSERT(slot < 0) return;
    IF_ASSERT(slot >= this->n_components) return;
    GS_ASSERT(this->components[slot] == NULL);
    this->components[slot] = component;
}

// returns component of type if available
// must cast to desired component manually
class Components::Component* Entity::get_component(ComponentType type) const
{
    int slot = entity_data->get_component_slot(this->type, type);
    if (slot < 0) return NULL;
    IF_ASSERT(slot >= this->n_components) return NULL;
    return this->components[slot];
}

class Components::Component* Entity::get_component_interface(ComponentInterfaceType interface) const
{
    int slot = entity_data->get_component_interface_slot(this->type, interface);
    if (slot < 0) return NULL;
    IF_ASSERT(slot >= this->n_components) return NULL;
    return this->components[slot];
}

void Entity::broadcastDeath()
{
    entity_destroy_StoC msg;
    msg.id = this->id;
    msg.type = this->type;
    msg.broadcast();
}

void Entity::init(size_t n_components)
{
    GS_ASSERT(this->components == NULL);
    GS_ASSERT(this->n_components == 0);
    this->n_components = n_components;
    this->components = (class Components::Component**)calloc(n_components, sizeof(*this->components));
}

void Entity::ready()
{
    for (int i=0; i<this->n_components; i++)
        this->components[i]->on_ready();
}

void Entity::destroy()
{
    for (int i=0; i<this->n_components; i++)
        this->components[i]->on_destroy();
}

void Entity::update()
{
    for (int i=0; i<this->n_components; i++)
        this->components[i]->on_update();
}

void Entity::after_update()
{
    for (int i=0; i<this->n_components; i++)
        this->components[i]->on_after_update();
}

void Entity::before_tick()
{
    for (int i=0; i<this->n_components; i++)
        this->components[i]->on_before_tick();
}

void Entity::tick()
{
    for (int i=0; i<this->n_components; i++)
        this->components[i]->on_tick();
}

void Entity::after_tick()
{
    for (int i=0; i<this->n_components; i++)
        this->components[i]->on_after_tick();
}

Entity::~Entity()
{
    this->release_components();
    free(this->components);
    this->components = NULL;
}

} // Entities
