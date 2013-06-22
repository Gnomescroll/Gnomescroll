#pragma once

///* Entity Interface */

//namespace Entities
//{

///* All of these must be defined for any entity */

////each entity type will have a single creation function
//Entity* create_some_object();

////and a ready function, called after creation and data loading (like setting position)
//void ready_some_object(Entity* entity);

////and a die function, called right before removing the entity from memory
//void die_some_object(Entity* entity);

//} // Entities



/* TEMPLATE (can copy/paste this and replace some_object with entity name */

#pragma once

namespace Entities
{

// forward decl
class Entity;

Entity* create_REPLACEME();
void ready_REPLACEME(Entity* entity);
void die_REPLACEME(Entity* entity);
//void tick_REPLACEME(Entity* entity);
//void update_REPLACEME(Entity* entity);

} // Entities

