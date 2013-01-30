#pragma once

#include <net_lib/net_StoC.hpp>

typedef enum
{
    NULL_ATTRIBUTE = 0xFF
}   AttributeType;

typedef enum
{
    NULL_ATTRIBUTE_GROUP,
    ATTRIBUTE_GROUP_AGENT,
    ATTRIBUTE_GROUP_SERVER,
}   AttributeGroup;

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

typedef void (*voidFunction)();

typedef int (*getInt)();
typedef float (*getFloat)();
typedef const char* (*getString)();

typedef bool (*setInt)(int);
typedef bool (*setFloat)(float);
typedef bool (*setString)(const char*);

const size_t MAX_ATTRIBUTES = NULL_ATTRIBUTE;

const size_t STRING_ATTRIBUTE_MAX_LENGTH = 63;

bool isValid(AttributeType type)
{
    return (type >= 0 && type < MAX_ATTRIBUTES);
}

namespace Attributes
{

/* Boilerplate */

void init();
void teardown();
void verify();

/* Registration */

AttributeType def(AttributeGroup group, const char* name);
void set_sync_type(AttributeType type, AttributeSyncType sync_type);
void set_location(AttributeType type, int* location);
void set_location(AttributeType type, float* location);
void set_location(AttributeType type, char** location);

void done_loading();

/* Network */

void init_packets();

class set_attribute_int_StoC:
    public FixedSizeNetPacketToClient<set_attribute_int_StoC>
{
    public:
        uint8_t attribute_type;
        int32_t value;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u8(&attribute_type, buff, buff_n, pack);
        pack_32(&value, buff, buff_n, pack);
    }

    inline void handle();
};

class set_attribute_float_StoC:
    public FixedSizeNetPacketToClient<set_attribute_float_StoC>
{
    public:
        uint8_t attribute_type;
        float value;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u8(&attribute_type, buff, buff_n, pack);
        pack_float(&value, buff, buff_n, pack);
    }

    inline void handle();
};

class set_attribute_string_StoC:
    public FixedSizeNetPacketToClient<set_attribute_string_StoC>
{
    public:

        uint8_t attribute_type;
        char value[STRING_ATTRIBUTE_MAX_LENGTH+1];

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u8(&attribute_type, buff, buff_n, pack);
        pack_string(value, STRING_ATTRIBUTE_MAX_LENGTH+1, buff, buff_n, pack);
    }

    inline void handle();
};

}   // Attributes
