#pragma once

/*
 * RULES:
 * DONT CHANGE STRING NAMES UNLESS YOU KNOW WHAT THE FUCK YOU ARE DOING AND HAVE A GOOD REASON
 * EVERY TIME YOU CHANGE A STRING NAME, YOU RISK BREAKING THINGS
 * YOU WILL BREAK THINGS IF YOU DON'T UPDATE THE RENAME SCHEME
 * IF THERE IS NO RENAME SCHEME, DON'T CHANGE A NAME
 * 
 * DONT USE ENUMS FOR DATA THAT LIVES OUTSIDE OF THE GAME SERVER INSTANCE
 */

// 9223372036854775807 -- signed 64bit int max (redis global id max)

#include <item/container/config/_state.hpp>

namespace serializer
{

// Map and mechs (flat file data)
#define DATA_PATH "./world/"
#define INVALID_DATA_SUBPATH "invalid/"
#define MAP_DATA_PATH  DATA_PATH "map/"
#define MECH_DATA_PATH DATA_PATH "mechs/"
#define MAP_DATA_EXT  ".map"
#define MECH_DATA_EXT ".mch"
const char default_map_file[] = MAP_DATA_PATH "map-" GS_STR(GS_VERSION) MAP_DATA_EXT;
const char mech_filename[] = MECH_DATA_PATH "mech-" GS_STR(GS_VERSION) MECH_DATA_EXT;


/*
 * REDIS DATA
 */

const int REDIS_TIMEOUT = 300;  // seconds
const int KEEP_ALIVE_RATE = (300*30)/2; // send keep-alive halfway

// Cached data queues
const unsigned int PLAYER_LOAD_DATA_LIST_INITIAL_SIZE = NetServer::HARD_MAX_CONNECTIONS;
const unsigned int PLAYER_LOAD_DATA_LIST_HARD_MAX = 128;
const unsigned int PLAYER_CONTAINER_LOAD_DATA_LIST_INITIAL_SIZE = PLAYER_LOAD_DATA_LIST_INITIAL_SIZE * N_PLAYER_CONTAINERS;
const unsigned int PLAYER_CONTAINER_LOAD_DATA_LIST_HARD_MAX = PLAYER_LOAD_DATA_LIST_HARD_MAX * N_PLAYER_CONTAINERS;

// DONT CHANGE THESE:

#define LOCATION_NAME_MAX_LENGTH 24

const unsigned int TAG_LENGTH = 3;
const unsigned int TAG_DELIMITER_LENGTH = 1;
const unsigned int PROPERTY_DELIMITER_LENGTH = 1;
const unsigned int COLOR_COMPONENT_DELIMITER_LENGTH = 1;

#define USER_ID_TAG         "UID"
#define GLOBAL_ID_TAG       "GID"
#define NAME_TAG            "NAM"
#define DURABILITY_TAG      "DUR"
#define STACK_SIZE_TAG      "STA"
#define CONTAINER_SLOT_TAG  "CSL"
#define CONTAINER_COUNT_TAG "CNT"
#define COLOR_TAG           "RGB"
#define SPAWNER_ID_TAG      "SPN"

#define TAG_DELIMITER             "="
#define PROPERTY_DELIMITER        ";"
#define COLOR_COMPONENT_DELIMITER ","

#define COLOR_COMPONENT_MAX_LENGTH 3  // color component (0-255)
#define COLOR_MAX_LENGTH ((COLOR_COMPONENT_MAX_LENGTH*3) + ((COLOR_COMPONENT_MAX_LENGTH-1)*COLOR_COMPONENT_DELIMITER_LENGTH))

#define SPAWNER_ID_MAX_LENGTH 4

#define PLAYER_CONTAINER_USER_ID_LENGTH 10
#define PLAYER_CONTAINER_CONTAINER_COUNT_LENGTH 3

#define ITEM_GLOBAL_ID_LENGTH 10
#define ITEM_DURABILITY_LENGTH 5
#define ITEM_STACK_SIZE_LENGTH 5
#define ITEM_CONTAINER_SLOT_LENGTH 3

const unsigned int PLAYER_FIELD_COUNT = 2;
const unsigned int PLAYER_CONTAINER_FIELD_COUNT = 3;
const unsigned int ITEM_FIELD_COUNT = 5;

// These values are just for reference and should not be relied upon
// A line's property count is subject to change, invalidating these lengths for anything saved before the change
// However, never make them SMALLER
const size_t PLAYER_LINE_LENGTH =
       PLAYER_FIELD_COUNT * (TAG_LENGTH + TAG_DELIMITER_LENGTH)
    + (PLAYER_FIELD_COUNT - 1) * PROPERTY_DELIMITER_LENGTH
    + COLOR_MAX_LENGTH
    + SPAWNER_ID_MAX_LENGTH;
    
const size_t PLAYER_CONTAINER_LINE_LENGTH =
       PLAYER_CONTAINER_FIELD_COUNT * (TAG_LENGTH + TAG_DELIMITER_LENGTH)
    + (PLAYER_CONTAINER_FIELD_COUNT - 1) * PROPERTY_DELIMITER_LENGTH
    + CONTAINER_NAME_MAX_LENGTH
    + PLAYER_CONTAINER_USER_ID_LENGTH
    + PLAYER_CONTAINER_CONTAINER_COUNT_LENGTH;

const size_t ITEM_LINE_LENGTH =
       ITEM_FIELD_COUNT * (TAG_LENGTH + TAG_DELIMITER_LENGTH)
    + (ITEM_FIELD_COUNT - 1) * PROPERTY_DELIMITER_LENGTH
    + ITEM_GLOBAL_ID_LENGTH
    + ITEM_NAME_MAX_LENGTH
    + ITEM_DURABILITY_LENGTH
    + ITEM_STACK_SIZE_LENGTH
    + ITEM_CONTAINER_SLOT_LENGTH;

const char PLAYER_DATA_FMT[] =
    COLOR_TAG       TAG_DELIMITER
        "%0" GS_STR(COLOR_COMPONENT_MAX_LENGTH) "d"
            COLOR_COMPONENT_DELIMITER
        "%0" GS_STR(COLOR_COMPONENT_MAX_LENGTH) "d"
            COLOR_COMPONENT_DELIMITER
        "%0" GS_STR(COLOR_COMPONENT_MAX_LENGTH) "d"
        PROPERTY_DELIMITER
    SPAWNER_ID_TAG  TAG_DELIMITER
        "%0" GS_STR(SPAWNER_ID_MAX_LENGTH) "d";

const char PLAYER_CONTAINER_HEADER_FMT[] =
    NAME_TAG            TAG_DELIMITER
        "%-" GS_STR(CONTAINER_NAME_MAX_LENGTH)               "s"
        PROPERTY_DELIMITER
    USER_ID_TAG         TAG_DELIMITER
        "%0" GS_STR(PLAYER_CONTAINER_USER_ID_LENGTH)         "d"
        PROPERTY_DELIMITER
    CONTAINER_COUNT_TAG TAG_DELIMITER
        "%0" GS_STR(PLAYER_CONTAINER_CONTAINER_COUNT_LENGTH) "d";
    
const char ITEM_FMT[] =
    GLOBAL_ID_TAG      TAG_DELIMITER
        "%0" GS_STR(ITEM_GLOBAL_ID_LENGTH)      "d"
        PROPERTY_DELIMITER
    NAME_TAG           TAG_DELIMITER
        "%-" GS_STR(ITEM_NAME_MAX_LENGTH)       "s"
        PROPERTY_DELIMITER
    DURABILITY_TAG     TAG_DELIMITER
        "%0" GS_STR(ITEM_DURABILITY_LENGTH)     "d"
        PROPERTY_DELIMITER
    STACK_SIZE_TAG     TAG_DELIMITER
        "%0" GS_STR(ITEM_STACK_SIZE_LENGTH)     "d"
        PROPERTY_DELIMITER
    CONTAINER_SLOT_TAG TAG_DELIMITER
        "%0" GS_STR(ITEM_CONTAINER_SLOT_LENGTH) "d";

#define PLAYER_REDIS_KEY_PREFIX "player:"

#define CONTAINER_LOCATION_NAME "container"
#define PARTICLE_LOCATION_NAME  "particle"

#define PLAYER_CONTAINER_LOCATION_PREFIX PLAYER_REDIS_KEY_PREFIX

#define PLAYER_HAND_LOCATION_SUBNAME         "hand"
#define PLAYER_TOOLBELT_LOCATION_SUBNAME     "toolbelt"
#define PLAYER_INVENTORY_LOCATION_SUBNAME    "inventory"
#define PLAYER_SYNTHESIZER_LOCATION_SUBNAME  "synthesizer"
#define PLAYER_ENERGY_TANKS_LOCATION_SUBNAME "energy_tanks"

#define PLAYER_HAND_LOCATION_NAME         PLAYER_CONTAINER_LOCATION_PREFIX PLAYER_HAND_LOCATION_SUBNAME
#define PLAYER_TOOLBELT_LOCATION_NAME     PLAYER_CONTAINER_LOCATION_PREFIX PLAYER_TOOLBELT_LOCATION_SUBNAME
#define PLAYER_INVENTORY_LOCATION_NAME    PLAYER_CONTAINER_LOCATION_PREFIX PLAYER_INVENTORY_LOCATION_SUBNAME
#define PLAYER_SYNTHESIZER_LOCATION_NAME  PLAYER_CONTAINER_LOCATION_PREFIX PLAYER_SYNTHESIZER_LOCATION_SUBNAME
#define PLAYER_ENERGY_TANKS_LOCATION_NAME PLAYER_CONTAINER_LOCATION_PREFIX PLAYER_ENERGY_TANKS_LOCATION_SUBNAME

// The following are not safe to store -- they are only valid per compilation. Only strings are safe

typedef enum
{
    LN_NONE,
    LN_CONTAINER,
    LN_PARTICLE,    
    LN_PLAYER_HAND,
    LN_PLAYER_INVENTORY,
    LN_PLAYER_ENERGY_TANKS,
    LN_PLAYER_TOOLBELT,
    LN_PLAYER_SYNTHESIZER,
} LocationNameID;

const char* get_location_name(LocationNameID loc_id)
{
    switch (loc_id)
    {        
        case LN_CONTAINER:
            return CONTAINER_LOCATION_NAME;
        case LN_PARTICLE:
            return PARTICLE_LOCATION_NAME;
        case LN_PLAYER_HAND:
            return PLAYER_HAND_LOCATION_NAME;
        case LN_PLAYER_ENERGY_TANKS:
            return PLAYER_ENERGY_TANKS_LOCATION_NAME;
        case LN_PLAYER_INVENTORY:
            return PLAYER_INVENTORY_LOCATION_NAME;
        case LN_PLAYER_SYNTHESIZER:
            return PLAYER_SYNTHESIZER_LOCATION_NAME;
        case LN_PLAYER_TOOLBELT:
            return PLAYER_TOOLBELT_LOCATION_NAME;
        case LN_NONE:
        default:
            GS_ASSERT(false);
            return NULL;
    }
    GS_ASSERT(false);
    return NULL;
}

LocationNameID get_container_location_name_id(ItemContainerType container_type)
{
    switch (container_type)
    {
        case AGENT_INVENTORY:
            return LN_PLAYER_INVENTORY;
        case AGENT_TOOLBELT:
            return LN_PLAYER_TOOLBELT;
        case AGENT_SYNTHESIZER:
            return LN_PLAYER_SYNTHESIZER;
        case AGENT_ENERGY_TANKS:
            return LN_PLAYER_ENERGY_TANKS;
        case AGENT_HAND:
            return LN_PLAYER_HAND;

        case CONTAINER_TYPE_STORAGE_BLOCK_SMALL:
        case CONTAINER_TYPE_CRAFTING_BENCH_UTILITY:
        case CONTAINER_TYPE_CRYOFREEZER_SMALL:
        case CONTAINER_TYPE_SMELTER_ONE:
        case CONTAINER_TYPE_CRUSHER:
            return LN_CONTAINER;
        
        case CONTAINER_TYPE_NONE:
        default:
            GS_ASSERT(false);
            return LN_NONE;
    }
    GS_ASSERT(false);
    return LN_NONE;
}

inline bool is_valid_location_name_char(char c)
{
    return (isalnum(c) || c == ':' || c == '_' || c == '-');
}

bool is_valid_location_name(const char* name)
{
    int i = 0;
    char c;
    while (i < LOCATION_NAME_MAX_LENGTH && (c = name[i]) != '\0')
    {
        if (!is_valid_location_name_char(c))
            return false;
        i++;
    }
    return (i < LOCATION_NAME_MAX_LENGTH || name[i] == '\0');
}


void verify_config()
{
    GS_ASSERT_ABORT(strcmp(CONTAINER_LOCATION_NAME, "container") == 0);
    GS_ASSERT_ABORT(strcmp(PARTICLE_LOCATION_NAME, "particle") == 0);
    GS_ASSERT_ABORT(strcmp(PLAYER_INVENTORY_LOCATION_NAME, "player:inventory") == 0);
    GS_ASSERT_ABORT(strcmp(PLAYER_ENERGY_TANKS_LOCATION_NAME, "player:energy_tanks") == 0);
    GS_ASSERT_ABORT(strcmp(PLAYER_SYNTHESIZER_LOCATION_NAME, "player:synthesizer") == 0);
    GS_ASSERT_ABORT(strcmp(PLAYER_TOOLBELT_LOCATION_NAME, "player:toolbelt") == 0);
    GS_ASSERT_ABORT(strcmp(PLAYER_HAND_LOCATION_NAME, "player:hand") == 0);

    GS_ASSERT_ABORT(strcmp(get_location_name(LN_CONTAINER), "container") == 0);
    GS_ASSERT_ABORT(strcmp(get_location_name(LN_PARTICLE), "particle") == 0);
    GS_ASSERT_ABORT(strcmp(get_location_name(LN_PLAYER_INVENTORY), "player:inventory") == 0);
    GS_ASSERT_ABORT(strcmp(get_location_name(LN_PLAYER_ENERGY_TANKS), "player:energy_tanks") == 0);
    GS_ASSERT_ABORT(strcmp(get_location_name(LN_PLAYER_SYNTHESIZER), "player:synthesizer") == 0);
    GS_ASSERT_ABORT(strcmp(get_location_name(LN_PLAYER_TOOLBELT), "player:toolbelt") == 0);
    GS_ASSERT_ABORT(strcmp(get_location_name(LN_PLAYER_HAND), "player:hand") == 0);

    // check that all the expected containers are loaded
    // this *could* be in item/container/config, but since the only reason this matters
    // is because of the seriousness of the serializer, i'll leave it here
    GS_ASSERT_ABORT(ItemContainer::container_attributes != NULL);

    bool agent_inventory_found = false;
    bool agent_toolbelt_found = false;
    bool agent_synthesizer_found = false;
    bool agent_energy_tanks_found = false;
    bool agent_hand_found = false;

    // make sure all container types are loaded
    
    for (int i=0; i<MAX_CONTAINER_TYPES; i++)
    {
        ContainerAttributes* attr = ItemContainer::container_attributes[i];
        if (attr == NULL || !attr->loaded || !attr->attached_to_agent) continue;
        switch (attr->type)
        {
            case AGENT_INVENTORY:
                agent_inventory_found = true;
                break;
            case AGENT_TOOLBELT:
                agent_toolbelt_found = true;
                break;
            case AGENT_SYNTHESIZER:
                agent_synthesizer_found = true;
                break;
            case AGENT_ENERGY_TANKS:
                agent_energy_tanks_found = true;
                break;
            case AGENT_HAND:
                agent_hand_found = true;
                break;

            default:
                break;
        }
    }

    GS_ASSERT_ABORT(agent_inventory_found);
    GS_ASSERT_ABORT(agent_toolbelt_found);
    GS_ASSERT_ABORT(agent_synthesizer_found);
    GS_ASSERT_ABORT(agent_energy_tanks_found);
    GS_ASSERT_ABORT(agent_hand_found);

    // length of location names
    GS_ASSERT_ABORT(strlen(CONTAINER_LOCATION_NAME) <= LOCATION_NAME_MAX_LENGTH);
    GS_ASSERT_ABORT(strlen(PARTICLE_LOCATION_NAME) <= LOCATION_NAME_MAX_LENGTH);
    GS_ASSERT_ABORT(strlen(PLAYER_INVENTORY_LOCATION_NAME) <= LOCATION_NAME_MAX_LENGTH);
    GS_ASSERT_ABORT(strlen(PLAYER_ENERGY_TANKS_LOCATION_NAME) <= LOCATION_NAME_MAX_LENGTH);
    GS_ASSERT_ABORT(strlen(PLAYER_SYNTHESIZER_LOCATION_NAME) <= LOCATION_NAME_MAX_LENGTH);
    GS_ASSERT_ABORT(strlen(PLAYER_TOOLBELT_LOCATION_NAME) <= LOCATION_NAME_MAX_LENGTH);
    GS_ASSERT_ABORT(strlen(PLAYER_HAND_LOCATION_NAME) <= LOCATION_NAME_MAX_LENGTH);

    GS_ASSERT_ABORT(is_valid_location_name(CONTAINER_LOCATION_NAME));
    GS_ASSERT_ABORT(is_valid_location_name(PARTICLE_LOCATION_NAME));
    GS_ASSERT_ABORT(is_valid_location_name(PLAYER_INVENTORY_LOCATION_NAME));
    GS_ASSERT_ABORT(is_valid_location_name(PLAYER_ENERGY_TANKS_LOCATION_NAME));
    GS_ASSERT_ABORT(is_valid_location_name(PLAYER_SYNTHESIZER_LOCATION_NAME));
    GS_ASSERT_ABORT(is_valid_location_name(PLAYER_TOOLBELT_LOCATION_NAME));
    GS_ASSERT_ABORT(is_valid_location_name(PLAYER_HAND_LOCATION_NAME));

    GS_ASSERT_ABORT(PROPERTY_DELIMITER_LENGTH == 1);
    GS_ASSERT_ABORT(TAG_DELIMITER_LENGTH == 1);
    GS_ASSERT_ABORT(COLOR_COMPONENT_DELIMITER_LENGTH == 1);

    // semicolon is reserved for redis key format, which may need to link back to the redis key
    GS_ASSERT_ABORT(strstr(TAG_DELIMITER, ":") == NULL);
    GS_ASSERT_ABORT(strstr(TAG_DELIMITER, " ") == NULL);
    GS_ASSERT_ABORT(strstr(TAG_DELIMITER, PROPERTY_DELIMITER) == NULL);
    GS_ASSERT_ABORT(strstr(TAG_DELIMITER, COLOR_COMPONENT_DELIMITER) == NULL);
    GS_ASSERT_ABORT(strlen(TAG_DELIMITER) == TAG_DELIMITER_LENGTH);

    GS_ASSERT_ABORT(strstr(PROPERTY_DELIMITER, ":") == NULL);
    GS_ASSERT_ABORT(strstr(PROPERTY_DELIMITER, " ") == NULL);
    GS_ASSERT_ABORT(strstr(PROPERTY_DELIMITER, TAG_DELIMITER) == NULL);
    GS_ASSERT_ABORT(strstr(PROPERTY_DELIMITER, COLOR_COMPONENT_DELIMITER) == NULL);
    GS_ASSERT_ABORT(strlen(PROPERTY_DELIMITER) == PROPERTY_DELIMITER_LENGTH);
    
    GS_ASSERT_ABORT(strstr(COLOR_COMPONENT_DELIMITER, ":") == NULL);
    GS_ASSERT_ABORT(strstr(COLOR_COMPONENT_DELIMITER, " ") == NULL);
    GS_ASSERT_ABORT(strstr(COLOR_COMPONENT_DELIMITER, TAG_DELIMITER) == NULL);
    GS_ASSERT_ABORT(strstr(COLOR_COMPONENT_DELIMITER, PROPERTY_DELIMITER) == NULL);
    GS_ASSERT_ABORT(strlen(COLOR_COMPONENT_DELIMITER) == COLOR_COMPONENT_DELIMITER_LENGTH);

    GS_ASSERT_ABORT(strlen(USER_ID_TAG) == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(GLOBAL_ID_TAG) == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(NAME_TAG) == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(DURABILITY_TAG) == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(STACK_SIZE_TAG) == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(CONTAINER_SLOT_TAG) == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(CONTAINER_COUNT_TAG) == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(COLOR_TAG) == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(SPAWNER_ID_TAG) == TAG_LENGTH);
    
    // THIS NUMBER CAN NEVER GO LOWER
    GS_ASSERT_ABORT(ITEM_LINE_LENGTH >= 71);
    GS_ASSERT_ABORT(PLAYER_CONTAINER_LINE_LENGTH >= 51);
    GS_ASSERT_ABORT(PLAYER_LINE_LENGTH >= 24);
}

}   // serializer
