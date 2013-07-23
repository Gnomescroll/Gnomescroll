#pragma once

#include <entity/network/_interface.hpp>

namespace Entities
{

void init_config();
void teardown_config();

size_t get_entity_max(EntityType type);
entityLoad get_entity_load_method(EntityType type);
CreatePacketDelegate* get_entity_create_packet_delegate(EntityType type);
StatePacketDelegate* get_entity_state_packet_delegate(EntityType type);
bool entity_is_networked(EntityType type);
const char* get_entity_name(EntityType type);
const char* get_entity_description(EntityType type);
const char* get_entity_pretty_name(EntityType type);
EntityType get_entity_type(const char* name);
inline bool is_valid_entity_name(const char* name);
const char* get_compatible_entity_name(const char* name);

}   // Entities
