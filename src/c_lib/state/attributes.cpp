#include "attributes.hpp"

#include <common/dat/attributes.hpp>

namespace World
{

class WorldAttributes: public Attributes::AttributesHolder
{
    public:

        int time_of_day;
        int temperature;

    WorldAttributes() :
        Attributes::AttributesHolder()
    {
    }
};

static WorldAttributes* attributes;

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

static void start_registration()
{
    AttributeGroup group = Attributes::start_registration();
    attributes->set_attribute_group(group);
}

void register_attributes()
{
    start_registration();

    attribute_def("time_of_day", 0);
    set_location(&attributes->time_of_day);
    set_sync_type(ATTRIBUTE_SYNC_TYPE_ALL);

    attribute_def("temperature", 40);
    set_location(&attributes->temperature);
    set_sync_type(ATTRIBUTE_SYNC_TYPE_ALL);

    Attributes::end_registration();
}

/*********************
 * API
 *********************/

#define SET_ATTRIBUTE(KEYTYPE, TYPE) \
    void set_attribute(KEYTYPE key, TYPE value) \
    { \
        Attributes::set(attributes->attribute_group, key, value); \
    }

#define GET_ATTRIBUTE(KEYTYPE, TYPE, NAME, RETVAL) \
    TYPE get_attribute_##NAME(KEYTYPE key) \
    { \
        return Attributes::get_##NAME(attributes->attribute_group, key); \
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
    GS_ASSERT(attributes == NULL);
    attributes = new WorldAttributes;
}

void teardown_attributes()
{
    if (attributes != NULL) delete attributes;
}


}   // World

