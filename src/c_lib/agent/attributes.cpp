#include "attributes.hpp"

#include <common/dat/attributes.hpp>
#include <common/dat/modifiers.hpp>
#include <common/common.hpp>

namespace Agents
{

// forward decl
template <typename Type> static void attribute_def(const char* name, Type value);
template <typename Type> static void set_limits(Type lower, Type upper);
template <typename Type> static void set_lower_limit(Type lower);
template <typename Type> static void set_upper_limit(Type upper);

/*******************
 * Main Registration
 *******************/

static void _register_attributes()
{
    attribute_def("max_health", 100);

    attribute_def("health", 100);
    set_limits(0, 140);
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


AttributeGroup base_stats = NULL_ATTRIBUTE_GROUP;
AttributeGroup* stats;

static AgentModifierList* agent_modifiers;
static struct ModifierSum** modifier_sums;


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

template <typename Type>
static void set_lower_limit(Type lower)
{
    Attributes::set_lower_limit(attr_type, lower);
}

template <typename Type>
static void set_upper_limit(Type upper)
{
    Attributes::set_upper_limit(attr_type, upper);
}

template <typename Type>
static void set_limits(Type lower, Type upper)
{
    set_lower_limit(lower);
    set_upper_limit(upper);
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

static void start_registration(AttributeGroup& stats)
{
    stats = Attributes::start_registration();
    GS_ASSERT(stats != NULL_ATTRIBUTE_GROUP);
}

void register_attributes()
{
    start_registration(base_stats);
    _register_attributes();
    Attributes::set_sync_type(base_stats, SYNC_TYPE_ALL);
    Attributes::end_registration();
    for (int i=0; i<MAX_AGENTS; i++)
    {
        start_registration(stats[i]);
        _register_attributes();
        Attributes::set_sync_to(stats[i], (ClientID)i);
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
    static bool set_base_attribute(KEYTYPE key, TYPE value) \
    { \
        return Attributes::set(base_stats, key, value); \
    }

#define GET_ATTRIBUTE(KEYTYPE, TYPE, NAME, RETVAL) \
    TYPE get_attribute_##NAME(AgentID agent_id, KEYTYPE key) \
    { \
        IF_ASSERT(!isValid(agent_id)) return RETVAL; \
        return Attributes::get_##NAME(stats[agent_id], key); \
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
    int amount, float percent)
{
    IF_ASSERT(value_type != ATTRIBUTE_VALUE_INT &&
              value_type != ATTRIBUTE_VALUE_FLOAT)
        return false;
    if (value_type == ATTRIBUTE_VALUE_INT)
    {
        int stat = get_attribute_int(agent_id, attribute_type);
        int base_stat = get_base_attribute_int(attribute_type);
        stat = stat + amount + int(round_half_from_zero(percent * float(base_stat)));
        return set_attribute(agent_id, attribute_type, stat);
    }
    else
    if (value_type == ATTRIBUTE_VALUE_FLOAT)
    {
        float stat = get_attribute_float(agent_id, attribute_type);
        float base_stat = get_base_attribute_float(attribute_type);
        stat = stat + amount + percent * base_stat;
        return set_attribute(agent_id, attribute_type, stat);
    }
    else
    {
        GS_ASSERT(false);
    }
    return false;
}

static bool _change_temporary_modifier(AgentID agent_id, const Modifier* modifier, int direction)
{
    IF_ASSERT(!isValid(agent_id)) return false;
    AttributeValueType value_type = Attributes::get_value_type(
        stats[agent_id], modifier->attribute_type);
    IF_ASSERT(value_type != ATTRIBUTE_VALUE_INT &&
              value_type != ATTRIBUTE_VALUE_FLOAT)
        return false;
    if (value_type == ATTRIBUTE_VALUE_INT)
    {
        int stat = get_base_attribute_int(modifier->attribute_type);
        if (modifier->change_type == MODIFIER_CHANGE_AMOUNT)
            stat += direction * modifier->amount;
        else
        if (modifier->change_type == MODIFIER_CHANGE_PERCENT)
            stat += direction * int(round_half_from_zero(modifier->percent * float(stat)));
        return set_attribute(agent_id, modifier->attribute_type, stat);
    }
    else
    if (value_type == ATTRIBUTE_VALUE_FLOAT)
    {
        float stat = get_base_attribute_float(modifier->attribute_type);
        if (modifier->change_type == MODIFIER_CHANGE_AMOUNT)
            stat += direction * modifier->amount;
        else
        if (modifier->change_type == MODIFIER_CHANGE_PERCENT)
            stat += direction * modifier->percent * stat;
        return set_attribute(agent_id, modifier->attribute_type, stat);
    }
    else
    {
        GS_ASSERT(false);
    }
    return false;
}

static inline bool _add_temporary_modifier(AgentID agent_id, const Modifier* modifier)
{
    // when equipment is added/removed, or when duration is added/remove
    return _change_temporary_modifier(agent_id, modifier, 1);
}


static inline bool _remove_temporary_modifier(AgentID agent_id, const Modifier* modifier)
{
    return _change_temporary_modifier(agent_id, modifier, -1);
}

static bool _add_instant_modifier(AgentID agent_id, const Modifier* modifier)
{
    IF_ASSERT(!isValid(agent_id)) return false;
    AttributeValueType value_type = Attributes::get_value_type(
        stats[agent_id], modifier->attribute_type);
    IF_ASSERT(value_type != ATTRIBUTE_VALUE_INT &&
              value_type != ATTRIBUTE_VALUE_FLOAT)
        return false;
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
            if (m->event_type == MODIFIER_EVENT_DURATION)
                _remove_temporary_modifier(agent_id, m);
            agent_modifiers[agent_id].destroy(m->id);
            continue;
        }
    }
}

static void _apply_cumulative_modifiers(AgentID agent_id)
{
    AttributeGroup group = stats[agent_id];
    size_t count = Attributes::get_attribute_count(group);
    for (size_t i=0; i<count; i++)
    {
        int amount = modifier_sums[agent_id][i].amount;
        float percent = modifier_sums[agent_id][i].percent;
        if (!amount && !percent) continue;
        AttributeValueType value_type = Attributes::get_value_type(group, (AttributeType)i);
        IF_ASSERT(value_type != ATTRIBUTE_VALUE_INT &&
                  value_type != ATTRIBUTE_VALUE_FLOAT) continue;
        _apply_cumulative_modifier(agent_id, (AttributeType)i, value_type, amount, percent);
    }
}

static bool _load_temporary_modifier(AgentID agent_id, const Modifier* modifier)
{
    IF_ASSERT(!isValid(agent_id)) return false;
    AgentModifier* agent_modifier = agent_modifiers[agent_id].create();
    IF_ASSERT(agent_modifier == NULL) return false;
    agent_modifier->load_modifier(modifier);
    _add_temporary_modifier(agent_id, modifier);
    return true;
}

static bool _apply_instant_agent_modifier(AgentID agent_id, const Modifier* modifier)
{
    IF_ASSERT(!isValid(agent_id)) return false;
    return _add_instant_modifier(agent_id, modifier);
}

static void _equipment_item_change(AgentID agent_id, ItemID item_id,
                                    bool (*changer)(AgentID, const Modifier*))
{
    const ModifierList* modifiers = Item::get_item_modifiers(item_id);
    IF_ASSERT(modifiers == NULL) return;
    for (size_t i=0; i<modifiers->max; i++)
    {
        const Modifier* m = &modifiers->objects[i];
        if (m->id == modifiers->null_id) continue;
        changer(agent_id, m);
    }
}

void add_equipment_item_callback(AgentID agent_id, ItemID item_id)
{
    _equipment_item_change(agent_id, item_id, &_add_temporary_modifier);
}

void remove_equipment_item_callback(AgentID agent_id, ItemID item_id)
{
    _equipment_item_change(agent_id, item_id, &_remove_temporary_modifier);
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
        _apply_timed_modifiers(agent->id);
        _apply_cumulative_modifiers(agent->id);
    }
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
    modifier_sums = (struct ModifierSum**)calloc(MAX_AGENTS, sizeof(struct ModifierSum));
    for (int i=0; i<MAX_AGENTS; i++)
        modifier_sums[i] = (struct ModifierSum*)calloc(MAX_ATTRIBUTES, sizeof(struct ModifierSum));
}

void teardown_attributes()
{
    if (stats != NULL) free(stats);
    if (agent_modifiers != NULL) delete[] agent_modifiers;
    if (modifier_sums != NULL)
    {
        for (int i=0; i<MAX_AGENTS; i++)
            if (modifier_sums[i] != NULL)
                free(modifier_sums[i]);
        free(modifier_sums);
    }
}

void reset_attributes(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    memset(&modifier_sums[agent_id], 0, MAX_ATTRIBUTES*sizeof(struct ModifierSum));
    agent_modifiers[agent_id].flush();
    Attributes::copy_from(stats[agent_id], base_stats);
}


}   // Agents
