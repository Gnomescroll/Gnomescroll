#include "attributes.hpp"

#include <common/dat/attributes.hpp>

namespace World
{

//forward decl
template <typename Type> static void attribute_def(const char* name, Type value);
static void set_sync_type(AttributeSyncType sync_type);

/*********************
 * Registration
 *********************/

static void _register_attributes()
{
    attribute_def("time_of_day", 0);
    set_sync_type(ATTRIBUTE_SYNC_TYPE_ALL);

    attribute_def("temperature", 40);
    set_sync_type(ATTRIBUTE_SYNC_TYPE_ALL);
}

/*********************
 * State
 *********************/

AttributeGroup attributes = NULL_ATTRIBUTE_GROUP;

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

void register_attributes()
{
    attributes = Attributes::start_registration();
    GS_ASSERT(attributes != NULL_ATTRIBUTE_GROUP);
    _register_attributes();
    Attributes::end_registration();
}

/*********************
 * Network
 *********************/

#if DC_SERVER
void send_attributes_to_client(ClientID client_id)
{
    Attributes::send_to_client(attributes, client_id);
}
#endif

/*********************
 * API
 *********************/

#define SET_ATTRIBUTE(KEYTYPE, TYPE) \
    void set_attribute(KEYTYPE key, TYPE value) \
    { \
        Attributes::set(attributes, key, value); \
    }

#define GET_ATTRIBUTE(KEYTYPE, TYPE, NAME, RETVAL) \
    TYPE get_attribute_##NAME(KEYTYPE key) \
    { \
        return Attributes::get_##NAME(attributes, key); \
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
    GS_ASSERT(attributes == NULL_ATTRIBUTE_GROUP);
}

void teardown_attributes()
{
}


}   // World

