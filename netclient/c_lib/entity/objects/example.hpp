//#pragma once

///* Object Interface */

//namespace Objects
//{

///* All of these must be defined for any object */

////each object type will have a single creation function
//Object* create_some_object();

////and a ready function, called after creation and data loading (like setting position)
//void ready_some_object(Object* object);

////and a die function, called right before removing the object from memory
//void die_some_object(Object* object);

//} // Objects



/* TEMPLATE (can copy/paste this and replace some_object with object name */

#pragma once

namespace Objects
{

// forward decl
class Object;

Object* create_REPLACEME();
void ready_REPLACEME(Object* object);
void die_REPLACEME(Object* object);

} // Objects

