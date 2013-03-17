#include "attributes.hpp"

#include <common/dat/attributes.hpp>
#include <common/dat/modifiers.hpp>
#include <common/common.hpp>
#include <agent/attribute_callbacks.hpp>

namespace Agents
{

// forward decl
template <typename Type> static void attribute_def(const char* name, Type value);
static void set_limits(int lower, int upper);
static void set_limits(float lower, float upper);
static void set_limits(const char* lower, int upper);
static void set_limits(const char* lower, float upper);
static void set_limits(int lower, const char* upper);
static void set_limits(float lower, const char* upper);
static void set_limits(const char* lower, const char* upper);
static void set_limits(const char* lower, const char* upper);
static void set_lower_limit(int lower);
static void set_lower_limit(float lower);
static void set_lower_limit(const char* lower);
static void set_upper_limit(int upper);
static void set_upper_limit(float upper);
static void set_upper_limit(const char* upper);
static void add_set_callback(setInt);
static void add_set_callback(setFloat);
static void add_set_callback(setString);
static void add_get_callback(getInt);
static void add_get_callback(getFloat);
static void add_get_callback(getString);

/*******************
 * Main Registration
 *******************/

static void _register_attributes()
{
    attribute_def("max_health", 40);
    set_lower_limit(0);

    attribute_def("health", 40);
    set_limits(0, "max_health");    //use health_max instead of max_health
    add_set_callback(&health_changed);

    attribute_def("hunger_max", 10);  //use hunger_max instead of hunger_max
    set_limits(10, 10);

    attribute_def("hunger", 0);
    set_limits(0, "hunger_max");

    attribute_def("rad_exposure_max", RAD_EXPOSURE_MAX);
    set_limits(RAD_EXPOSURE_MAX, RAD_EXPOSURE_MAX);

    attribute_def("rad_exposure", 0);
    set_limits(0, "rad_exposure_max"); //400 is max bar


    attribute_def("rad_level_max", 0xF);
    set_limits(0xF, 0xF);

    attribute_def("rad_level", 0);
    set_limits(0, "rad_level_max");
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

    void flush()
    {
        for (size_t i=0; i<this->max; i++)
            if (this->objects[i].id != this->null_id)
                this->destroy(this->objects[i].id);
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

struct StatDifference
{
    AttributeValueType value_type;
    union
    {
        int i;
        float f;
    };
};

AttributeGroup base_stats = NULL_ATTRIBUTE_GROUP;
AttributeGroup* agent_stats;
AttributeGroup* agent_base_stats;

static AgentModifierList* agent_modifiers;
static struct StatDifference* stat_differences;
static struct ModifierSum* modifier_sums;


/*********************
 * Registration Helpers
 *********************/

#define SET_BASE_ATTRIBUTE_FORWARD_DECLARE(KEYTYPE, TYPE) \
    static bool set_base_attribute(KEYTYPE key, TYPE value);
SET_BASE_ATTRIBUTE_FORWARD_DECLARE(AttributeType, int);
SET_BASE_ATTRIBUTE_FORWARD_DECLARE(AttributeType, float);
SET_BASE_ATTRIBUTE_FORWARD_DECLARE(AttributeType, const char*);
SET_BASE_ATTRIBUTE_FORWARD_DECLARE(const char*, int);
SET_BASE_ATTRIBUTE_FORWARD_DECLARE(const char*, float);
SET_BASE_ATTRIBUTE_FORWARD_DECLARE(const char*, const char*);
#undef SET_BASE_ATTRIBUTE_FORWARD_DECLARE

static AttributeType attr_type = NULL_ATTRIBUTE;

template <typename Type>
static void attribute_def(const char* name, Type value)
{
    attr_type = Attributes::def(name, value);
    GS_ASSERT(attr_type != NULL_ATTRIBUTE);
}

#define SET_LIMITS_BODY \
    { set_lower_limit(lower); \
      set_upper_limit(upper); }\

#define SET_LIMITS(TYPE) \
    static void set_limits(TYPE lower, TYPE upper) SET_LIMITS_BODY \
    static void set_limits(TYPE lower, const char* upper) SET_LIMITS_BODY  \
    static void set_limits(const char* lower, TYPE upper) SET_LIMITS_BODY

#define SET_LOWER_LIMIT_BODY \
    { Attributes::set_lower_limit(attr_type, lower); }

#define SET_LOWER_LIMIT(TYPE) \
    static void set_lower_limit(TYPE lower) SET_LOWER_LIMIT_BODY

#define SET_UPPER_LIMIT_BODY \
    { Attributes::set_upper_limit(attr_type, upper); }

#define SET_UPPER_LIMIT(TYPE) \
    static void set_upper_limit(TYPE upper) SET_UPPER_LIMIT_BODY

SET_LIMITS(int)
SET_LIMITS(float)
static void set_limits(const char* lower, const char* upper) SET_LIMITS_BODY

SET_LOWER_LIMIT(int)
SET_LOWER_LIMIT(float)
SET_LOWER_LIMIT(const char*)
SET_UPPER_LIMIT(int)
SET_UPPER_LIMIT(float)
SET_UPPER_LIMIT(const char*)

#undef SET_LIMITS_BODY
#undef SET_LIMITS
#undef SET_LOWER_LIMIT_BODY
#undef SET_LOWER_LIMIT
#undef SET_UPPER_LIMIT_BODY
#undef SET_UPPER_LIMIT

static void add_set_callback(setInt cb)
{
    Attributes::add_set_callback(attr_type, cb);
}

static void add_set_callback(setFloat cb)
{
    Attributes::add_set_callback(attr_type, cb);
}

static void add_set_callback(setString cb)
{
    Attributes::add_set_callback(attr_type, cb);
}

static void add_get_callback(getInt cb)
{
    Attributes::add_get_callback(attr_type, cb);
}

static void add_get_callback(getFloat cb)
{
    Attributes::add_get_callback(attr_type, cb);
}

static void add_get_callback(getString cb)
{
    Attributes::add_get_callback(attr_type, cb);
}


static void test_registration()
{
    int val = get_base_attribute_int("max_health");
    set_base_attribute("max_health", 75);
    GS_ASSERT(75 == get_base_attribute_int("max_health"));
    set_base_attribute("max_health", val);

    const AgentID agent_id = (AgentID)(MAX_AGENTS/2);
    val = get_attribute_int(agent_id, "health");
    set_attribute(agent_id, "health", 50);
    GS_ASSERT(50 == get_attribute_int(agent_id, "health"));
    set_attribute(agent_id, "health", val);
}

void register_attributes()
{
    base_stats = Attributes::start_registration();
    GS_ASSERT(base_stats != NULL_ATTRIBUTE_GROUP);
    _register_attributes();
    Attributes::set_sync_type(base_stats, SYNC_TYPE_ALL);
    Attributes::end_registration();
    for (int i=0; i<MAX_AGENTS; i++)
    {
        agent_base_stats[i] = Attributes::start_registration();
        GS_ASSERT(agent_base_stats[i] != NULL_ATTRIBUTE_GROUP);
        _register_attributes();
        Attributes::set_sync_type(agent_base_stats[i], SYNC_TYPE_PRIVATE);
        Attributes::end_registration();
    }
    for (int i=0; i<MAX_AGENTS; i++)
    {
        agent_stats[i] = Attributes::start_registration();
        GS_ASSERT(agent_stats[i] != NULL_ATTRIBUTE_GROUP);
        _register_attributes();
        Attributes::set_sync_to(agent_stats[i], (ClientID)i);
        Attributes::end_registration();
    }

    #if !PRODUCTION
    //test_registration();
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
    Attributes::send_to_client(agent_stats[agent_id], client_id);
}
#endif

/*********************
 * API
 *********************/

#define SET_ATTRIBUTE(KEYTYPE, TYPE) \
    bool set_attribute(AgentID agent_id, KEYTYPE key, TYPE value) \
    { \
        IF_ASSERT(!isValid(agent_id)) return false; \
        return Attributes::set(agent_stats[agent_id], key, value); \
    } \
    static bool set_base_attribute(KEYTYPE key, TYPE value) \
    { \
        return Attributes::set(base_stats, key, value); \
    }

#define GET_ATTRIBUTE(KEYTYPE, TYPE, NAME, RETVAL) \
    TYPE get_attribute_##NAME(AgentID agent_id, KEYTYPE key) \
    { \
        IF_ASSERT(!isValid(agent_id)) return RETVAL; \
        return Attributes::get_##NAME(agent_stats[agent_id], key); \
    } \
    TYPE get_base_attribute_##NAME(KEYTYPE key) \
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
//  sum percent deltas
//      base_val = (percent + 1) * base_stat
//  sum amount deltas
//      base_val += amount
//  stat = stat + delta + percent * base_stat

static bool _apply_cumulative_modifier(AgentID agent_id,
    AttributeType attribute_type, AttributeValueType value_type,
    ModifierSum sum)
{   // add sum to base_stat to get agent_base_stat
    IF_ASSERT(!isValid(agent_id)) return false;
    IF_ASSERT(value_type != ATTRIBUTE_VALUE_INT &&
              value_type != ATTRIBUTE_VALUE_FLOAT) return false;
    if (value_type == ATTRIBUTE_VALUE_INT)
    {
        int stat = get_base_attribute_int(attribute_type);
        stat = sum.amount + int(round_half_from_zero((sum.percent+1) * float(stat)));
        return Attributes::set(agent_base_stats[agent_id], attribute_type, stat);
    }
    else
    if (value_type == ATTRIBUTE_VALUE_FLOAT)
    {
        float stat = get_base_attribute_float(attribute_type);
        stat = sum.amount + (sum.percent+1) * stat;
        return Attributes::set(agent_base_stats[agent_id], attribute_type, stat);
    }
    else
    {
        GS_ASSERT(false);
    }
    return false;
}

static bool _add_instant_modifier(AgentID agent_id, const Modifier* modifier)
{
    IF_ASSERT(!isValid(agent_id)) return false;
    AttributeValueType value_type = Attributes::get_value_type(
        agent_stats[agent_id], modifier->attribute_type);
    if (value_type == ATTRIBUTE_VALUE_INT)
    {
        int stat = get_attribute_int(agent_id, modifier->attribute_type);
        if (modifier->change_type == MODIFIER_CHANGE_AMOUNT)
            stat += modifier->amount;
        else
        if (modifier->change_type == MODIFIER_CHANGE_PERCENT)
            stat += int(round_half_from_zero(modifier->percent * float(stat)));
        return set_attribute(agent_id, modifier->attribute_type, stat);
    }
    else
    if (value_type == ATTRIBUTE_VALUE_FLOAT)
    {
        float stat = get_attribute_float(agent_id, modifier->attribute_type);
        if (modifier->change_type == MODIFIER_CHANGE_AMOUNT)
            stat += modifier->amount;
        else
        if (modifier->change_type == MODIFIER_CHANGE_PERCENT)
            stat += modifier->percent * stat;
        return set_attribute(agent_id, modifier->attribute_type, stat);
    }
    else
    {
        GS_ASSERT(false);
    }
    return false;
}

static void _apply_timed_modifiers(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    if (!agent_modifiers[agent_id].ct) return;
    for (size_t i=0; i<agent_modifiers[agent_id].max; i++)
    {
        AgentModifier* m = &agent_modifiers[agent_id].objects[i];
        if (m->id == agent_modifiers[agent_id].null_id) continue;
        m->tick++;
        if (m->event_type == MODIFIER_EVENT_PERIODIC &&
            m->tick % m->period == 0)
        {
            _add_instant_modifier(agent_id, m);
        }
        if (m->tick >= m->duration)
        {
            GS_ASSERT(m->duration > 0);
            agent_modifiers[agent_id].destroy(m->id);
            continue;
        }
    }
}

static void _apply_stat_difference(AgentID agent_id, AttributeType type, struct StatDifference diff)
{
    IF_ASSERT(!isValid(agent_id)) return;
    if (diff.value_type == ATTRIBUTE_VALUE_INT)
    {
        int value = Attributes::get_int(agent_base_stats[agent_id], type);
        value += diff.i;
        set_attribute(agent_id, type, value);
    }
    else
    if (diff.value_type == ATTRIBUTE_VALUE_FLOAT)
    {
        float value = Attributes::get_float(agent_base_stats[agent_id], type);
        value += diff.f;
        set_attribute(agent_id, type, value);
    }
    else
    {
        GS_ASSERT(false);
    }
}

static void _apply_stat_differences(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    size_t count = Attributes::get_attribute_count(base_stats);
    for (size_t i=0; i<count; i++)
        if (stat_differences[i].value_type != NULL_ATTRIBUTE_VALUE_TYPE)
            _apply_stat_difference(agent_id, (AttributeType)i, stat_differences[i]);
}

static void _apply_cumulative_modifiers(AgentID agent_id)
{   // modifiers + base_stats = agent_base_stats
    IF_ASSERT(!isValid(agent_id)) return;
    size_t count = Attributes::get_attribute_count(base_stats);
    for (size_t i=0; i<count; i++)
    {
        AttributeValueType value_type = Attributes::get_value_type(base_stats, (AttributeType)i);
        IF_ASSERT(value_type != ATTRIBUTE_VALUE_INT &&
                  value_type != ATTRIBUTE_VALUE_FLOAT) continue;
        _apply_cumulative_modifier(agent_id, (AttributeType)i, value_type, modifier_sums[i]);
    }
}

static void _sum_status_modifiers(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    const Agent* agent = get_agent(agent_id);
    IF_ASSERT(agent == NULL) return;
    const AgentModifierList* modifiers = &agent_modifiers[agent_id];
    for (size_t i=0; i<modifiers->max; i++)
    {
        const AgentModifier* m = modifiers->get((ModifierID)i);
        if (m == NULL) continue;
        if (m->event_type != MODIFIER_EVENT_DURATION) continue;
        IF_ASSERT(m->attribute_type == NULL_ATTRIBUTE) continue;
        modifier_sums[m->attribute_type].amount += m->amount;
        modifier_sums[m->attribute_type].percent += m->percent;
    }
}

static void _sum_equipment_modifiers(AgentID agent_id)
{
    ItemContainerID equipment_id = ItemContainer::get_agent_equipment(agent_id);
    const ItemContainer::ItemContainerInterface* equipment = ItemContainer::get_container(equipment_id);
    IF_ASSERT(equipment == NULL) return;
    for (int i=0; i<equipment->slot_max; i++)
    {
        ItemID item_id = equipment->slot[i];
        if (item_id == NULL_ITEM) continue;
        const ModifierList* modifiers = Item::get_item_modifiers(item_id);
        IF_ASSERT(modifiers == NULL) continue;
        for (size_t j=0; j<modifiers->max; j++)
        {
            const Modifier* m = modifiers->get((ModifierID)j);
            if (m == NULL) continue;
            IF_ASSERT(m->attribute_type == NULL_ATTRIBUTE) continue;
            modifier_sums[m->attribute_type].amount += m->amount;
            modifier_sums[m->attribute_type].percent += m->percent;
        }
    }
}

static void _sum_cumulative_modifiers(AgentID agent_id)
{   // sum all temporary modifiers and equipment modifiers
    memset(modifier_sums, 0, MAX_ATTRIBUTES * sizeof(struct ModifierSum));
    _sum_status_modifiers(agent_id);
    _sum_equipment_modifiers(agent_id);
}

static void _calculate_stat_differences(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    size_t count = Attributes::get_attribute_count(base_stats);
    memset(stat_differences, 0, MAX_ATTRIBUTES * sizeof(struct StatDifference));
    for (size_t i=0; i<count; i++)
        stat_differences[i].value_type = NULL_ATTRIBUTE_VALUE_TYPE;
    for (size_t i=0; i<count; i++)
    {
        AttributeType type = (AttributeType)i;
        AttributeValueType value_type = Attributes::get_value_type(agent_stats[agent_id], type);
        if (value_type == ATTRIBUTE_VALUE_STRING) continue;
        if (value_type == ATTRIBUTE_VALUE_INT)
        {
            int diff = (Attributes::get_int(agent_stats[agent_id], type) -
                        Attributes::get_int(agent_base_stats[agent_id], type));
            stat_differences[i].value_type = ATTRIBUTE_VALUE_INT;
            stat_differences[i].i = diff;
        }
        else
        if (value_type == ATTRIBUTE_VALUE_FLOAT)
        {
            float diff = (Attributes::get_float(agent_stats[agent_id], type) -
                          Attributes::get_float(agent_base_stats[agent_id], type));
            stat_differences[i].value_type = ATTRIBUTE_VALUE_FLOAT;
            stat_differences[i].f = diff;
        }
        else
        {
            GS_ASSERT(false);
        }
    }
}

static bool _load_temporary_modifier(AgentID agent_id, const Modifier* modifier)
{
    IF_ASSERT(!isValid(agent_id)) return false;
    AgentModifier* agent_modifier = agent_modifiers[agent_id].create();
    IF_ASSERT(agent_modifier == NULL) return false;
    agent_modifier->load_modifier(modifier);
    return true;
}

static bool _apply_instant_agent_modifier(AgentID agent_id, const Modifier* modifier)
{
    IF_ASSERT(!isValid(agent_id)) return false;
    return _add_instant_modifier(agent_id, modifier);
}

bool apply_agent_modifier(AgentID agent_id, const Modifier* modifier)
{
    IF_ASSERT(!isValid(agent_id)) return false;
    IF_ASSERT(modifier == NULL) return false;
    IF_ASSERT(modifier->event_type != MODIFIER_EVENT_INSTANT &&
              modifier->event_type != MODIFIER_EVENT_PERIODIC &&
              modifier->event_type != MODIFIER_EVENT_DURATION) return false;
    if (modifier->event_type == MODIFIER_EVENT_INSTANT)
        return _apply_instant_agent_modifier(agent_id, modifier);
    return _load_temporary_modifier(agent_id, modifier);
}

void apply_agent_modifiers()
{
    for (int i=0; i<MAX_AGENTS; i++)
    {
        Agent* agent = get_agent((AgentID)i);
        if (agent == NULL) continue;
        // compute difference between agent_stats and agent_base_stats
        _calculate_stat_differences(agent->id);
        // sum up equipment and status modifiers
        _sum_cumulative_modifiers(agent->id);
        // apply the final modifiers to base_stats to get agent_base_stats
        _apply_cumulative_modifiers(agent->id);
        // apply the original differences back to agent_base_stats
        _apply_stat_differences(agent->id);
        // trigger any periodic modifiers and remove expired ones
        _apply_timed_modifiers(agent->id);
    }
}
#endif

/*********************
 * Boilerplate
 *********************/

void init_attributes()
{
    GS_ASSERT(base_stats == NULL_ATTRIBUTE_GROUP);
    GS_ASSERT(agent_stats == NULL);
    GS_ASSERT(agent_base_stats == NULL);
    GS_ASSERT(agent_modifiers == NULL);
    GS_ASSERT(modifier_sums == NULL);
    GS_ASSERT(stat_differences == NULL);
    agent_stats = (AttributeGroup*)malloc(MAX_AGENTS * sizeof(AttributeGroup));
    agent_base_stats = (AttributeGroup*)malloc(MAX_AGENTS * sizeof(AttributeGroup));
    for (int i=0; i<MAX_AGENTS; i++)
    {
        agent_stats[i] = NULL_ATTRIBUTE_GROUP;
        agent_base_stats[i] = NULL_ATTRIBUTE_GROUP;
    }
    agent_modifiers = new AgentModifierList[MAX_AGENTS];
    modifier_sums = (struct ModifierSum*)calloc(MAX_ATTRIBUTES, sizeof(struct ModifierSum));
    stat_differences = (struct StatDifference*)calloc(MAX_ATTRIBUTES, sizeof(struct StatDifference));
    for (size_t i=0; i<MAX_ATTRIBUTES; i++)
        stat_differences[i].value_type = NULL_ATTRIBUTE_VALUE_TYPE;
}

void teardown_attributes()
{
    free(agent_stats);
    free(agent_base_stats);
    free(stat_differences);
    delete[] agent_modifiers;
    free(modifier_sums);
}

void reset_attributes(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    agent_modifiers[agent_id].flush();
    Attributes::copy_from(agent_base_stats[agent_id], base_stats);
    Attributes::copy_from(agent_stats[agent_id], agent_base_stats[agent_id]);
}

class Agent* get_agent_from_attribute_group(AttributeGroup group)
{
    if (group < agent_stats[0] || group > agent_stats[MAX_AGENTS-1])
        return NULL;
    int offset = group - agent_stats[0];
    if (offset < 0 || offset >= MAX_AGENTS)
        return NULL;
    return get_agent((AgentID)offset);
}

}   // Agents
