/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

namespace Agents
{

extern AttributeGroup base_stats;
extern AttributeGroup* stats;

// Boilerplate
void init_attributes();
void teardown_attributes();
void register_attributes();

void reset_attributes(AgentID agent_id);

class Agent* get_agent_from_attribute_group(AttributeGroup group);

#if DC_SERVER
// Modifiers
void apply_agent_modifiers();
bool apply_agent_modifier(AgentID agent_id, const Modifier* modifier);

// Network
void send_attributes_to_client(ClientID client_id);
#endif

// API
bool set_attribute(AgentID agent_id, AttributeType key, int value);
bool set_attribute(AgentID agent_id, AttributeType key, float value);
bool set_attribute(AgentID agent_id, AttributeType key, const char* value);
bool set_attribute(AgentID agent_id, const char* key, int value);
bool set_attribute(AgentID agent_id, const char* key, float value);
bool set_attribute(AgentID agent_id, const char* key, const char* value);

int get_attribute_int(AgentID agent_id, AttributeType key);
float get_attribute_float(AgentID agent_id, AttributeType key);
const char* get_attribute_string(AgentID agent_id, AttributeType key);
int get_attribute_int(AgentID agent_id, const char* key);
float get_attribute_float(AgentID agent_id, const char* key);
const char* get_attribute_string(AgentID agent_id, const char* key);

int get_base_attribute_int(AttributeType key);
float get_base_attribute_float(AttributeType key);
const char* get_base_attribute_string(AttributeType key);
int get_base_attribute_int(const char* key);
float get_base_attribute_float(const char* key);
const char* get_base_attribute_string(const char* key);

}   // Agents
