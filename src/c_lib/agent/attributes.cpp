#include "attributes.hpp"

#include <common/dat/attributes.hpp>

namespace Agents
{

class Stats
{
    public:
        int max_health;
        int health;
};

class StatModifiers
{
    private:
        Stats* base;

    public:
        int max_health;
        int health;

    int get_max_health()
    {
        return this->base->max_health + this->max_health;
    }

    int get_health()
    {
        return this->base->health + this->health;
    }


    void _set_base_stats(Stats* base)
    {
        GS_ASSERT(base != NULL);
        this->base = base;
    }

    StatModifiers()
    {}
};

static Stats* base_stats;
static StatModifiers* stats;

/*********************
 * Registration
 *********************/

static AttributeType attr_type = NULL_ATTRIBUTE;

static void attribute_def(const char* name, int value)
{
    attr_type = Attributes::def(ATTRIBUTE_GROUP_AGENT, name, value);
    GS_ASSERT(attr_type != NULL_ATTRIBUTE);
}

static void attribute_def(const char* name, float value)
{
    attr_type = Attributes::def(ATTRIBUTE_GROUP_AGENT, name, value);
    GS_ASSERT(attr_type != NULL_ATTRIBUTE);
}

static void attribute_def(const char* name, const char* value)
{
    attr_type = Attributes::def(ATTRIBUTE_GROUP_AGENT, name, value);
    GS_ASSERT(attr_type != NULL_ATTRIBUTE);
}

static void set_sync_type(AttributeSyncType sync_type)
{
    Attributes::set_sync_type(attr_type, sync_type);
}

static void set_location(int* location)
{
    Attributes::set_location(attr_type, location);
}

static void set_location(float* location)
{
    Attributes::set_location(attr_type, location);
}

static void set_location(char** location)
{
    Attributes::set_location(attr_type, location);
}

void register_attributes()
{
    attribute_def("max_health", 100);
    set_location(&base_stats->max_health);
    set_sync_type(ATTRIBUTE_SYNC_TYPE_ALL);

    attribute_def("health", 100);
    set_location(&base_stats->health);
    set_sync_type(ATTRIBUTE_SYNC_TYPE_ALL);

    Attributes::done_loading();

    printf("%s: %d\n", "max_health", Attributes::get_int("max_health"));
    printf("%s: %d\n", "health", Attributes::get_int("health"));
    Attributes::set("max_health", 25);
    Attributes::set("health", 50);
    printf("%s: %d\n", "max_health", Attributes::get_int("max_health"));
    printf("%s: %d\n", "health", Attributes::get_int("health"));
}

/*********************
 * API
 *********************/

int get_agent_max_health(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return base_stats->max_health;
    return stats[agent_id].get_max_health();
}

int get_agent_health(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return 0;
    return stats[agent_id].get_health();
}

/*********************
 * Boilerplate
 *********************/

void init_attributes()
{
    GS_ASSERT(base_stats == NULL);
    GS_ASSERT(stats == NULL);

    base_stats = new Stats;
    memset(base_stats, 0, sizeof(Stats));

    stats = new StatModifiers[MAX_AGENTS];
    memset(stats, 0, MAX_AGENTS * sizeof(StatModifiers));
    for (int i=0; i<MAX_AGENTS; i++)
        stats[i]._set_base_stats(base_stats);
}

void teardown_attributes()
{
    if (base_stats != NULL) delete base_stats;
    if (stats != NULL) delete[] stats;
}

}   // Agents
