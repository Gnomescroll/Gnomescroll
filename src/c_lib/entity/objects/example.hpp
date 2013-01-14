#pragma once

///* Entity Interface */

//namespace Entities
//{

///* All of these must be defined for any object */

////each object type will have a single creation function
//Entity* create_some_object();

////and a ready function, called after creation and data loading (like setting position)
//void ready_some_object(Entity* object);

////and a die function, called right before removing the object from memory
//void die_some_object(Entity* object);

//} // Entities



/* TEMPLATE (can copy/paste this and replace some_object with object name */

#pragma once

namespace Entities
{

// forward decl
class Entity;

Entity* create_REPLACEME();
void ready_REPLACEME(Entity* object);
void die_REPLACEME(Entity* object);
//void tick_REPLACEME(Entity* object);
//void update_REPLACEME(Entity* object);

} // Entities

