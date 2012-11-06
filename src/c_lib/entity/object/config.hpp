#pragma once

#include <entity/object/types.hpp>

namespace Entities
{

void init_config();
void teardown_config();

unsigned int get_object_max(EntityType type);
entityLoad get_object_load_method(EntityType type);
entityCreate get_object_create_method(EntityType type);
entityReady get_object_ready_method(EntityType type);
entityDie get_object_die_method(EntityType type);

}   // Entities
