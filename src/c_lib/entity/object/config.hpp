#pragma once

#include <entity/object/types.hpp>

namespace Objects
{

void init_config();
void teardown_config();

unsigned int get_object_max(ObjectType type);
objectLoad get_object_load_method(ObjectType type);
objectCreate get_object_create_method(ObjectType type);
objectReady get_object_ready_method(ObjectType type);
objectDie get_object_die_method(ObjectType type);

}   // Objects
