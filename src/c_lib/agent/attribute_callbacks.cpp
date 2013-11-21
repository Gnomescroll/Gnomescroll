/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "attribute_callbacks.hpp"

#include <agent/attributes.hpp>

namespace Agents
{

#if DC_CLIENT
int health_changed(AttributeGroup group, AttributeType type, int old_health, int new_health)
{
    Agent* agent = get_agent_from_attribute_group(group);
    if (agent == NULL) return new_health;
    if (new_health > old_health)
        agent->event.healed(new_health - old_health);
    else
    if (new_health < old_health)
        agent->event.took_damage(old_health - new_health);
    return new_health;
}
#endif

#if DC_SERVER
int health_changed(AttributeGroup group, AttributeType type, int old_health, int new_health)
{
    return new_health;
}
#endif

}   // Agents
