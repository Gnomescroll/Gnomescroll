#pragma once

#include <entity/constants.hpp>

namespace Components
{

// forward declaration
class Component;
Component* get_switch(ComponentType type);
void release_switch(Component* component);

/* API */

Component* get(ComponentType type)
{
    return get_switch(type);
}

void release(Component* component)
{
    release_switch(component);
}

ComponentInterfaceType get_interface_for_component(ComponentType component);

void init();
void teardown();

#if DC_SERVER
EntityID get_spawner_for_user(UserID user_id);
void revoke_owned_entities(AgentID owner_id);
#endif

} // Components
