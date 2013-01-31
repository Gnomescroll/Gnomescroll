#include "attributes.hpp"

#include <common/dat/attributes.hpp>
#include <common/dat/modifiers.hpp>

namespace Agents
{

class Stats
{
    public:
        AttributesID attributes_id;

        int max_health;
        int health;

    void set_attributes_id(AttributesID id)
    {
        GS_ASSERT(this->attributes_id == NULL_ATTRIBUTES_ID);
        GS_ASSERT(id != NULL_ATTRIBUTES_ID);
        this->attributes_id = id;
    }

    Stats() :
        attributes_id(NULL_ATTRIBUTES_ID)
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
    attr_type = Attributes::def(ATTRIBUTE_GROUP_AGENT, name, value);
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
    AttributesID id = Attributes::start_registration();
    stats->set_attributes_id(id);
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

static void test_registration(Stats* stats)
{
    int val = Attributes::get_int(stats->attributes_id, "max_health");
    Attributes::set(stats->attributes_id, "max_health", 75);
    GS_ASSERT(75 == Attributes::get_int(stats->attributes_id, "max_health"));
    Attributes::set(stats->attributes_id, "max_health", val);

    val = Attributes::get_int(stats->attributes_id, "health");
    Attributes::set(stats->attributes_id, "health", 50);
    GS_ASSERT(50 == Attributes::get_int(stats->attributes_id, "health"));
    Attributes::set(stats->attributes_id, "health", val);
}

void register_attributes()
{
    register_attributes(base_stats);
    for (int i=0; i<MAX_AGENTS; i++)
        register_attributes(&stats[i]);

    #if !PRODUCTION
    test_registration(base_stats);
    test_registration(&stats[MAX_AGENTS/2]);
    #endif
}

/*********************
 * API
 *********************/

#define SET_ATTRIBUTE(KEYTYPE, TYPE) \
    void set_attribute(AgentID agent_id, KEYTYPE key, TYPE value) \
    { \
        IF_ASSERT(!isValid(agent_id)) return; \
        Stats* s = &stats[agent_id]; \
        Attributes::set(s->attributes_id, key, value); \
    } \
    void set_attribute(KEYTYPE key, TYPE value) \
    { \
        Attributes::set(base_stats->attributes_id, key, value); \
    }

#define GET_ATTRIBUTE(KEYTYPE, TYPE, NAME, RETVAL) \
    TYPE get_##NAME(AgentID agent_id, KEYTYPE key) \
    { \
        IF_ASSERT(!isValid(agent_id)) return RETVAL; \
        Stats* s = &stats[agent_id]; \
        return Attributes::get_##NAME(s->attributes_id, key); \
    } \
    TYPE get_##NAME(KEYTYPE key) \
    { \
        return Attributes::get_##NAME(base_stats->attributes_id, key); \
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
