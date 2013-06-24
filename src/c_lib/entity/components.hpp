#pragma once

#include <entity/constants.hpp>

namespace Components
{

class Component;

/* API */

Component* get(ComponentType type);
void release(Component* component);
void call_lists();

ComponentInterfaceType get_interface_for_component(ComponentType component);

void init();
void init_config();
void teardown();

#if DC_SERVER
EntityID get_spawner_for_user(UserID user_id);
void revoke_owned_entities(AgentID owner_id);
void relax_positions();
#endif

} // Components
