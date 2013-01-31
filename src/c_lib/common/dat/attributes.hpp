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

typedef int (*getInt)();
typedef float (*getFloat)();
typedef const char* (*getString)();

typedef bool (*setInt)(int);
typedef bool (*setFloat)(float);
typedef bool (*setString)(const char*);

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

class AttributesHolder
{
    public:
        AttributeGroup attribute_group;

    void set_attribute_group(AttributeGroup group)
    {
        GS_ASSERT(this->attribute_group == NULL_ATTRIBUTE_GROUP);
        GS_ASSERT(group != NULL_ATTRIBUTE_GROUP);
        this->attribute_group = group;
    }

    AttributesHolder() :
        attribute_group(NULL_ATTRIBUTE_GROUP)
    {
    }
};

/* Meta API */

const char* get_name(AttributeGroup group, AttributeType type);
AttributeType get_type(AttributeGroup group, const char* name);

/* Read/write API */

AttributeGroup start_registration();
void end_registration();

void set(AttributeGroup group, AttributeType type, int value);
void set(AttributeGroup group, const char* name, int value);
void set(AttributeGroup group, AttributeType type, float value);
void set(AttributeGroup group, const char* name, float value);
void set(AttributeGroup group, AttributeType type, const char* value);
void set(AttributeGroup group, const char* name, const char* value);

int get_int(AttributeGroup group, AttributeType type);
int get_int(AttributeGroup group, const char* name);
float get_float(AttributeGroup group, AttributeType type);
float get_float(AttributeGroup group, const char* name);
const char* get_string(AttributeGroup group, AttributeType type);
const char* get_string(AttributeGroup group, const char* name);

/* Boilerplate */

void init();
void teardown();
void verify();

/* Registration */

AttributeType def(const char* name, int value);
AttributeType def(const char* name, float value);
AttributeType def(const char* name, const char* value);

void set_sync_type(AttributeType type, AttributeSyncType sync_type);
void set_location(AttributeType type, int* location);
void set_location(AttributeType type, float* location);
void set_location(AttributeType type, char** location);

void done_loading();

/* Network */

void init_packets();

#if DC_SERVER
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
