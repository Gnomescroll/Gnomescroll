#pragma once

#include <agent/constants.hpp>
#include <net_lib/net_StoC.hpp>

const size_t STRING_ATTRIBUTE_MAX_LENGTH = 63;

typedef enum
{
    NULL_ATTRIBUTE = 0xFF
}   AttributeType;

typedef enum
{
    NULL_ATTRIBUTE_VALUE_TYPE,
    ATTRIBUTE_VALUE_INT,
    ATTRIBUTE_VALUE_FLOAT,
    ATTRIBUTE_VALUE_STRING,
}   AttributeValueType;

typedef enum
{
    NULL_ATTRIBUTE_SYNC_TYPE,
    ATTRIBUTE_SYNC_TYPE_PRIVATE,
    ATTRIBUTE_SYNC_TYPE_PLAYER,
    ATTRIBUTE_SYNC_TYPE_ALL,
}   AttributeSyncType;

typedef enum
{
    NULL_ATTRIBUTE_GROUP = MAX_AGENTS + 2
}   AttributeGroup;

typedef void (*voidFunction)();

// Get callbacks; When an attribute is requested, the current attribute value is
// passed to the get callback, and the return value of the get callback is returned
// to the requester
typedef int (*getInt)(int);
typedef float (*getFloat)(float);
typedef const char* (*getString)(char*);

// Set callbacks; When an attribute is being set, the old value and new value
// are passed to the set callback, and the return value of the set callback
// is used as the new value. Any limits are applied to both the value sent to
// and the value returned from set_callback.
typedef int (*setInt)(int, int);
typedef float (*setFloat)(float, float);
typedef const char* (*setString)(const char*, const char*);

const size_t MAX_ATTRIBUTES = NULL_ATTRIBUTE;
const size_t MAX_ATTRIBUTE_GROUPS = NULL_ATTRIBUTE_GROUP;

bool isValid(AttributeType type)
{
    return (type >= 0 && type < MAX_ATTRIBUTES);
}

bool isValid(AttributeGroup group)
{
    return (group >= 0 && group < MAX_ATTRIBUTE_GROUPS);
}

namespace Attributes
{

/* Meta API */

const char* get_name(AttributeGroup group, AttributeType type);
AttributeType get_type(AttributeGroup group, const char* name);
size_t get_attribute_count(AttributeGroup group);
AttributeValueType get_value_type(AttributeGroup group, const char* name);
AttributeValueType get_value_type(AttributeGroup group, AttributeType type);

/* Read/write API */

AttributeGroup start_registration();
void end_registration();

bool set(AttributeGroup group, AttributeType type, int value);
bool set(AttributeGroup group, const char* name, int value);
bool set(AttributeGroup group, AttributeType type, float value);
bool set(AttributeGroup group, const char* name, float value);
bool set(AttributeGroup group, AttributeType type, const char* value);
bool set(AttributeGroup group, const char* name, const char* value);

int get_int(AttributeGroup group, AttributeType type);
int get_int(AttributeGroup group, const char* name);
float get_float(AttributeGroup group, AttributeType type);
float get_float(AttributeGroup group, const char* name);
const char* get_string(AttributeGroup group, AttributeType type);
const char* get_string(AttributeGroup group, const char* name);

void copy_from(AttributeGroup dest_group, AttributeGroup src_group);

/* Boilerplate */

void init();
void teardown();
void verify();

/* Registration */

AttributeType def(const char* name, int value);
AttributeType def(const char* name, float value);
AttributeType def(const char* name, const char* value);
void set_sync_type(AttributeType type, AttributeSyncType sync_type);
void add_get_callback(AttributeType type, getInt cb);
void add_get_callback(AttributeType type, getFloat cb);
void add_get_callback(AttributeType type, getString cb);
void add_set_callback(AttributeType type, setInt cb);
void add_set_callback(AttributeType type, setFloat cb);
void add_set_callback(AttributeType type, setString cb);
void set_lower_limit(AttributeType type, int lower);
void set_upper_limit(AttributeType type, int upper);
void set_lower_limit(AttributeType type, float lower);
void set_upper_limit(AttributeType type, float upper);
void done_loading();

/* Network */

void init_packets();

#if DC_SERVER
void set_sync_to(AttributeGroup group, ClientID client_id);
void send_to_client(AttributeGroup group, ClientID client_id);
void send_changes();
#endif

class set_attribute_int_StoC:
    public FixedSizeNetPacketToClient<set_attribute_int_StoC>
{
    public:
        uint8_t attribute_group;
        uint8_t attribute_type;
        int32_t value;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u8(&attribute_group, buff, buff_n, pack);
        pack_u8(&attribute_type, buff, buff_n, pack);
        pack_32(&value, buff, buff_n, pack);
    }

    inline void handle();
};

class set_attribute_float_StoC:
    public FixedSizeNetPacketToClient<set_attribute_float_StoC>
{
    public:
        uint8_t attribute_group;
        uint8_t attribute_type;
        float value;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u8(&attribute_group, buff, buff_n, pack);
        pack_u8(&attribute_type, buff, buff_n, pack);
        pack_float(&value, buff, buff_n, pack);
    }

    inline void handle();
};

class set_attribute_string_StoC:
    public FixedSizeNetPacketToClient<set_attribute_string_StoC>
{
    public:
        uint8_t attribute_group;
        uint8_t attribute_type;
        char value[STRING_ATTRIBUTE_MAX_LENGTH+1];

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u8(&attribute_group, buff, buff_n, pack);
        pack_u8(&attribute_type, buff, buff_n, pack);
        pack_string(value, STRING_ATTRIBUTE_MAX_LENGTH+1, buff, buff_n, pack);
    }

    inline void handle();
};

}   // Attributes
