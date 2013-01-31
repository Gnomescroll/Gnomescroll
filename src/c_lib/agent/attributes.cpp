#include "attributes.hpp"

#include <common/dat/attributes.hpp>
#include <common/dat/modifiers.hpp>

namespace Agents
{

class Stats: public Attributes::AttributesHolder
{
    public:
        int max_health;
        int health;

    Stats() :
        Attributes::AttributesHolder()
    {
    }
};

static Stats* base_stats;
static Stats* stats;

/*********************
 * Registration
 *********************/

static AttributeType attr_type = NULL_ATTRIBUTE;

template <typename Type>
static void attribute_def(const char* name, Type value)
{
    attr_type = Attributes::def(name, value);
    GS_ASSERT(attr_type != NULL_ATTRIBUTE);
}

template <typename Type>
static void set_location(Type location)
{
    Attributes::set_location(attr_type, location);
}

static void set_sync_type(AttributeSyncType sync_type)
{
    Attributes::set_sync_type(attr_type, sync_type);
}

static void start_registration(Stats* stats)
{
    AttributeGroup group = Attributes::start_registration();
    stats->set_attribute_group(group);
}

static void register_attributes(Stats* stats)
{
    start_registration(stats);

    attribute_def("max_health", 100);
    set_location(&stats->max_health);
    set_sync_type(ATTRIBUTE_SYNC_TYPE_ALL);

    attribute_def("health", 100);
    set_location(&stats->health);
    set_sync_type(ATTRIBUTE_SYNC_TYPE_ALL);

    Attributes::end_registration();
}

static void test_registration()
{
    int val = get_attribute_int("max_health");
    set_attribute("max_health", 75);
    GS_ASSERT(75 == get_attribute_int("max_health"));
    set_attribute("max_health", val);

    const AgentID agent_id = (AgentID)(MAX_AGENTS/2);
    val = get_attribute_int(agent_id, "health");
    set_attribute(agent_id, "health", 50);
    GS_ASSERT(50 == get_attribute_int(agent_id, "health"));
    set_attribute(agent_id, "health", val);
}

void register_attributes()
{
    register_attributes(base_stats);
    for (int i=0; i<MAX_AGENTS; i++)
    {
        register_attributes(&stats[i]);
        #if DC_SERVER
        Attributes::set_sync_to(stats[i].attribute_group, (ClientID)i);
        #endif
    }

    #if !PRODUCTION
    test_registration();
    #endif
}

/*********************
 * Network
 *********************/

#if DC_SERVER
void send_attributes_to_client(ClientID client_id)
{
    AgentID agent_id = NetServer::get_agent_id_for_client(client_id);
    IF_ASSERT(!isValid(agent_id)) return;
    Attributes::send_to_client(base_stats->attribute_group, client_id);
    Attributes::send_to_client(stats[agent_id].attribute_group, client_id);
}
#endif

/*********************
 * API
 *********************/

#define SET_ATTRIBUTE(KEYTYPE, TYPE) \
    void set_attribute(AgentID agent_id, KEYTYPE key, TYPE value) \
    { \
        IF_ASSERT(!isValid(agent_id)) return; \
        Stats* s = &stats[agent_id]; \
        Attributes::set(s->attribute_group, key, value); \
    } \
    void set_attribute(KEYTYPE key, TYPE value) \
    { \
        Attributes::set(base_stats->attribute_group, key, value); \
    }

#define GET_ATTRIBUTE(KEYTYPE, TYPE, NAME, RETVAL) \
    TYPE get_attribute_##NAME(AgentID agent_id, KEYTYPE key) \
    { \
        IF_ASSERT(!isValid(agent_id)) return RETVAL; \
        Stats* s = &stats[agent_id]; \
        return Attributes::get_##NAME(s->attribute_group, key); \
    } \
    TYPE get_attribute_##NAME(KEYTYPE key) \
    { \
        return Attributes::get_##NAME(base_stats->attribute_group, key); \
    }

SET_ATTRIBUTE(AttributeType, int);
SET_ATTRIBUTE(AttributeType, float);
SET_ATTRIBUTE(AttributeType, const char*);
SET_ATTRIBUTE(const char*, int);
SET_ATTRIBUTE(const char*, float);
SET_ATTRIBUTE(const char*, const char*);

GET_ATTRIBUTE(AttributeType, int, int, 1);
GET_ATTRIBUTE(AttributeType, float, float, 1.0f);
GET_ATTRIBUTE(AttributeType, const char*, string, NULL);
GET_ATTRIBUTE(const char*, int, int, 1);
GET_ATTRIBUTE(const char*, float, float, 1.0f);
GET_ATTRIBUTE(const char*, const char*, string, NULL);

#undef SET_ATTRIBUTE
#undef GET_ATTRIBUTE

/*********************
 * Boilerplate
 *********************/

void init_attributes()
{
    GS_ASSERT(base_stats == NULL);
    GS_ASSERT(stats == NULL);

    base_stats = new Stats;
    stats = new Stats[MAX_AGENTS];
}

void teardown_attributes()
{
    if (base_stats != NULL) delete base_stats;
    if (stats != NULL) delete[] stats;
}

}   // Agents
