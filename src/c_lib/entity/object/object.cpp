#include "object.hpp"

#include <entity/constants.hpp>
#include <entity/component/component.hpp>
#include <entity/network/packets.hpp>
#include <entity/components/pickup.hpp>
#include <entity/components/health/ttl.hpp>

namespace Entities
{

using Components::Component;

void Entity::add_component(int slot, class Component* component)
{
    IF_ASSERT(slot < 0) return;
    IF_ASSERT(slot >= this->n_components) return;
    GS_ASSERT(this->components[slot] == NULL);
    this->components[slot] = component;
}

// returns component of type if available
// must cast to desired component manually
class Component* Entity::get_component(ComponentType type)
{
    int slot = entity_data->get_component_slot(this->type, type);
    if (slot < 0) return NULL;
    GS_ASSERT(slot < this->n_components);
    return this->components[slot];
}

class Component* Entity::get_component_interface(ComponentInterfaceType interface)
{
    int slot = entity_data->get_component_interface_slot(this->type, interface);
    if (slot < 0) return NULL;
    GS_ASSERT(slot < this->n_components);
    return this->components[slot];
}

void Entity::broadcastDeath()
{
    object_destroy_StoC msg;
    msg.id = this->id;
    msg.type = this->type;
    msg.broadcast();
}

void Entity::init(int n_components)
{
    GS_ASSERT(this->components == NULL);
    GS_ASSERT(this->n_components == 0);
    IF_ASSERT(n_components < 0) n_components = 0;
    this->n_components = n_components;
    if (n_components <= 0) return;
    this->components = (class Component**)calloc(n_components, sizeof(class Component*));
}

Entity::~Entity()
{
    free(this->components);
}

} // Entities
