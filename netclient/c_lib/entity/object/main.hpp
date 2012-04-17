#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/object/object_list.hpp>

namespace Objects
{

extern ObjectList* object_list;

void init();
void teardown();

void tick();
void update();

void add_component_to_object(Object* object, ComponentType type);
void remove_component_from_object(Object* object, Component* component);

Object* create_switch(ObjectType type);
void ready_switch(Object* object);
void destroy_switch(Object* object);

} // Objects
