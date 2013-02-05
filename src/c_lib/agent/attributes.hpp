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
#define SET_ATTRIBUTE_HEADER(KEY, TYPE) \
    bool set_attribute(AgentID agent_id, KEY key, TYPE value);

#define GET_ATTRIBUTE_HEADER(KEY, TYPE, NAME) \
    TYPE get_attribute_##NAME(AgentID agent_id, KEY key); \
    TYPE get_base_attribute_##NAME(KEY key);

SET_ATTRIBUTE_HEADER(AttributeType, int);
SET_ATTRIBUTE_HEADER(AttributeType, float);
SET_ATTRIBUTE_HEADER(AttributeType, const char*);
SET_ATTRIBUTE_HEADER(const char*, int);
SET_ATTRIBUTE_HEADER(const char*, float);
SET_ATTRIBUTE_HEADER(const char*, const char*);

GET_ATTRIBUTE_HEADER(AttributeType, int, int);
GET_ATTRIBUTE_HEADER(AttributeType, float, float);
GET_ATTRIBUTE_HEADER(AttributeType, const char*, string);
GET_ATTRIBUTE_HEADER(const char*, int, int);
GET_ATTRIBUTE_HEADER(const char*, float, float);
GET_ATTRIBUTE_HEADER(const char*, const char*, string);

#undef SET_ATTRIBUTE_HEADER
#undef GET_ATTRIBUTE_HEADER

}   // Agents
