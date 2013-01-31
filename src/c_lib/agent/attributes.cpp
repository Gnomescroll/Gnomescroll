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
