#include "attributes.hpp"

#include <math.h>
#include <common/dat/attributes.hpp>
#include <common/dat/modifiers.hpp>
#include <common/common.hpp>

namespace Agents
{

// forward decl
template <typename Type> static void attribute_def(const char* name, Type value);
static void set_sync_type(AttributeSyncType sync_type);

/*******************
 * Main Registration
 *******************/

static void _register_attributes()
{
    attribute_def("max_health", 100);
    set_sync_type(ATTRIBUTE_SYNC_TYPE_ALL);

    attribute_def("health", 100);
    set_sync_type(ATTRIBUTE_SYNC_TYPE_ALL);
}

/*******************
 * Classes and state
 *******************/

class AgentModifier: public Modifier
{
    public:
        int tick;

    void load_modifier(const Modifier* modifier)
    {
        this->tick = 0;
        Modifier::copy_from(modifier);
    }

    AgentModifier(ModifierID id) :
        Modifier(id),
        tick(0)
    {}
};

class AgentModifierList: public ObjectList<AgentModifier, ModifierID>
{
    public:
    const char* name()
    {
        return "AgentModifierList";
    }

    AgentModifier* create()
    {
        return ObjectList<AgentModifier, ModifierID>::create();
    }

    AgentModifier* create(ModifierID id)
    {
        GS_ASSERT(false);
        return NULL;
    }

    AgentModifierList() :
        ObjectList<AgentModifier, ModifierID>(MAX_AGENT_MODIFIERS, NULL_MODIFIER)
    {}
};

struct ModifierSum
{
    int amount;
    float percent;
};


AttributeGroup base_stats = NULL_ATTRIBUTE_GROUP;
AttributeGroup* stats;

static AgentModifierList* agent_modifiers;
static struct ModifierSum* modifier_sums;


/*********************
 * Registration Helpers
 *********************/

static AttributeType attr_type = NULL_ATTRIBUTE;

template <typename Type>
static void attribute_def(const char* name, Type value)
{
    attr_type = Attributes::def(name, value);
    GS_ASSERT(attr_type != NULL_ATTRIBUTE);
}

static void set_sync_type(AttributeSyncType sync_type)
{
    Attributes::set_sync_type(attr_type, sync_type);
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

static void start_registration(AttributeGroup& stats)
{
    stats = Attributes::start_registration();
    GS_ASSERT(stats != NULL_ATTRIBUTE_GROUP);
}

void register_attributes()
{
    start_registration(base_stats);
    _register_attributes();
    Attributes::end_registration();
    for (int i=0; i<MAX_AGENTS; i++)
    {
        start_registration(stats[i]);
        _register_attributes();
        #if DC_SERVER
        Attributes::set_sync_to(stats[i], (ClientID)i);
        #endif
        Attributes::end_registration();
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
    Attributes::send_to_client(base_stats, client_id);
    Attributes::send_to_client(stats[agent_id], client_id);
}
#endif

/*********************
 * API
 *********************/

#define SET_ATTRIBUTE(KEYTYPE, TYPE) \
    bool set_attribute(AgentID agent_id, KEYTYPE key, TYPE value) \
    { \
        IF_ASSERT(!isValid(agent_id)) return false; \
        return Attributes::set(stats[agent_id], key, value); \
    } \
    bool set_attribute(KEYTYPE key, TYPE value) \
    { \
        return Attributes::set(base_stats, key, value); \
    }

#define GET_ATTRIBUTE(KEYTYPE, TYPE, NAME, RETVAL) \
    TYPE get_attribute_##NAME(AgentID agent_id, KEYTYPE key) \
    { \
        IF_ASSERT(!isValid(agent_id)) return RETVAL; \
        return Attributes::get_##NAME(stats[agent_id], key); \
    } \
    TYPE get_attribute_##NAME(KEYTYPE key) \
    { \
        return Attributes::get_##NAME(base_stats, key); \
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

#if DC_SERVER
/*********************
 * Modifiers
 *********************/

// read our modifiers
//  apply any time based ones
// read our modifiers and our equipment modifiers
//  sum percents
//      apply sum
//  sum deltas
//      apply deltas

static bool update_agent_attribute(AgentID agent_id,
    AttributeType attribute_type, AttributeValueType value_type,
    int amount, float percent)
{
    IF_ASSERT(value_type != ATTRIBUTE_VALUE_INT && value_type != ATTRIBUTE_VALUE_FLOAT)
        return false;
    if (value_type == ATTRIBUTE_VALUE_INT)
    {
        int val = get_attribute_int(agent_id, attribute_type);
        if (percent)
        {   // percents are defined as +/-%;
            float fval = float(val) * fabs(percent); // get amount to adjust by
            val += int(roundf(fval)*sgn(percent));   // round value and restore sign
        }
        if (amount)
            val += amount;
        return set_attribute(agent_id, attribute_type, val);
    }
    else
    if (value_type == ATTRIBUTE_VALUE_FLOAT)
    {
        float val = get_attribute_float(agent_id, attribute_type);
        if (percent)
            val += val * percent;
        if (amount)
            val += amount;
        return set_attribute(agent_id, attribute_type, val);
    }
    else
    {
        GS_ASSERT(false);
    }
    return false;
}

static void apply_timed_modifiers(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    for (size_t i=0; i<agent_modifiers[agent_id].max; i++)
    {
        AgentModifier* m = &agent_modifiers[agent_id].objects[i];
        if (m->id == agent_modifiers[agent_id].null_id) continue;
        if (m->tick >= m->duration)
        {
            GS_ASSERT(m->duration > 0);
            agent_modifiers[agent_id].destroy(m->id);
            continue;
        }
        AttributeValueType value_type = Attributes::get_value_type(stats[agent_id], m->attribute_type);
        if (m->tick % m->duration == 0)
            update_agent_attribute(agent_id, m->attribute_type, value_type,
                                   m->amount, m->percent);
        m->tick++;
    }
}

static void apply_cumulative_modifiers(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;

    memset(modifier_sums, 0, MAX_ATTRIBUTES*sizeof(struct ModifierSum));

    AttributeGroup group = stats[agent_id];
    size_t count = Attributes::get_attribute_count(group);

    // sum up modifier amounts
    for (size_t i=0; i<agent_modifiers[agent_id].max; i++)
    {
        const AgentModifier* m = &agent_modifiers[agent_id].objects[i];
        if (m->id == agent_modifiers[agent_id].null_id) continue;
        modifier_sums[m->attribute_type].percent += m->percent;
        modifier_sums[m->attribute_type].amount += m->amount;
    }

    // recalculate stats
    for (size_t i=0; i<count; i++)
    {
        int amount = modifier_sums[agent_id].amount;
        float percent = modifier_sums[agent_id].percent;
        if (!amount && !percent) continue;
        AttributeValueType value_type = Attributes::get_value_type(group, (AttributeType)i);
        IF_ASSERT(value_type != ATTRIBUTE_VALUE_INT && value_type != ATTRIBUTE_VALUE_FLOAT)
                continue;
        update_agent_attribute(agent_id, (AttributeType)i, value_type, amount, percent);
    }
}

void apply_agent_modifiers()
{
    for (int i=0; i<MAX_AGENTS; i++)
    {
        apply_timed_modifiers((AgentID)i);
        apply_cumulative_modifiers((AgentID)i);
    }
}

static bool add_agent_modifier(AgentID agent_id, const Modifier* modifier)
{
    IF_ASSERT(!isValid(agent_id)) return false;
    AgentModifier* agent_modifier = agent_modifiers[agent_id].create();
    IF_ASSERT(agent_modifier == NULL) return false;
    agent_modifier->load_modifier(modifier);
    return true;
}

static bool apply_instant_agent_modifier(AgentID agent_id, const Modifier* modifier)
{
    IF_ASSERT(!isValid(agent_id)) return false;
    AttributeValueType value_type = Attributes::get_value_type(
        stats[agent_id], modifier->attribute_type);
    return update_agent_attribute(agent_id, modifier->attribute_type, value_type,
                                  modifier->amount, modifier->percent);
}

bool apply_agent_modifier(AgentID agent_id, const Modifier* modifier)
{
    IF_ASSERT(!isValid(agent_id)) return false;
    IF_ASSERT(modifier == NULL) return false;
    IF_ASSERT(modifier->event_type != MODIFIER_EVENT_PERIODIC) return false;
    if (modifier->duration == 0)    // instant
        return apply_instant_agent_modifier(agent_id, modifier);
    return add_agent_modifier(agent_id, modifier);
}

#endif

/*********************
 * Boilerplate
 *********************/

void init_attributes()
{
    GS_ASSERT(base_stats == NULL_ATTRIBUTE_GROUP);
    GS_ASSERT(stats == NULL);
    GS_ASSERT(agent_modifiers == NULL);
    GS_ASSERT(modifier_sums == NULL);

    stats = (AttributeGroup*)malloc(MAX_AGENTS * sizeof(AttributeGroup));
    for (int i=0; i<MAX_AGENTS; stats[i++] = NULL_ATTRIBUTE_GROUP);
    agent_modifiers = new AgentModifierList[MAX_AGENTS];
    modifier_sums = (struct ModifierSum*)calloc(MAX_ATTRIBUTES, sizeof(struct ModifierSum));
}

void teardown_attributes()
{
    if (stats != NULL) free(stats);
    if (agent_modifiers != NULL) delete[] agent_modifiers;
    if (modifier_sums != NULL) free(modifier_sums);
}

}   // Agents
