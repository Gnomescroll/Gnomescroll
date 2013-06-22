#pragma once

#include <entity/entity/types.hpp>
#include <entity/network/main.hpp>

namespace Entities
{

void init_config();
void teardown_config();

size_t get_entity_max(EntityType type);
entityLoad get_entity_load_method(EntityType type);
entityReady get_entity_ready_method(EntityType type);
entityDie get_entity_die_method(EntityType type);
entityTick get_entity_tick_method(EntityType type);
entityUpdate get_entity_update_method(EntityType type);
CreatePacketDelegate* get_entity_create_packet_delegate(EntityType type);
StatePacketDelegate* get_entity_state_packet_delegate(EntityType type);
bool entity_is_networked(EntityType type);

}   // Entities
