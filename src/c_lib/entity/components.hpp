#pragma once

#include <entity/constants.hpp>

namespace Components
{

// forward declaration
class Component;
Component* get_switch (ComponentType type);
void release_switch (Component* component);

/* API */

Component* get(ComponentType type)
{
    return get_switch (type);
}

void release(Component* component)
{
    release_switch (component);
}

ComponentInterfaceType get_interface_for_component(ComponentType component);

void init();
void teardown();

} // Components
