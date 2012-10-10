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
const char default_map_file[] = MAP_DATA_PATH "map-" STR(GS_VERSION) MAP_DATA_EXT;
const char mech_filename[] = MECH_DATA_PATH "mech-" STR(GS_VERSION) MECH_DATA_EXT;


/*
 * REDIS DATA
 */

const int redis_timeout = 300;  // seconds
const int keep_alive_rate = (300*30)/2; // send keep-alive halfway

// Cached data queues
const unsigned int PLAYER_LOAD_DATA_LIST_HARD_MAX = 128;
const unsigned int PLAYER_CONTAINER_LOAD_DATA_LIST_HARD_MAX = 128;
const unsigned int PLAYER_ITEM_LOAD_DATA_LIST_HARD_MAX = 8192;
const unsigned int PLAYER_ITEM_SAVE_DATA_LIST_HARD_MAX = 8192;

const unsigned int EXPECTED_ITEM_HASH_FIELDS = 7;

// DONT CHANGE THESE
#define CONTAINER_LOCATION_NAME "container"
#define PARTICLE_LOCATION_NAME "particle"

#define PLAYER_CONTAINER_LOCATION_PREFIX "player:"

#define PLAYER_INVENTORY_LOCATION_SUBNAME "inventory"
#define PLAYER_ENERGY_TANKS_LOCATION_SUBNAME "energy_tanks"
#define PLAYER_SYNTHESIZER_LOCATION_SUBNAME "synthesizer"
#define PLAYER_TOOLBELT_LOCATION_SUBNAME "toolbelt"
#define PLAYER_HAND_LOCATION_SUBNAME "hand"

#define PLAYER_INVENTORY_LOCATION_NAME PLAYER_CONTAINER_LOCATION_PREFIX PLAYER_INVENTORY_LOCATION_SUBNAME
#define PLAYER_ENERGY_TANKS_LOCATION_NAME PLAYER_CONTAINER_LOCATION_PREFIX PLAYER_ENERGY_TANKS_LOCATION_SUBNAME
#define PLAYER_SYNTHESIZER_LOCATION_NAME PLAYER_CONTAINER_LOCATION_PREFIX PLAYER_SYNTHESIZER_LOCATION_SUBNAME
#define PLAYER_TOOLBELT_LOCATION_NAME PLAYER_CONTAINER_LOCATION_PREFIX PLAYER_TOOLBELT_LOCATION_SUBNAME
#define PLAYER_HAND_LOCATION_NAME PLAYER_CONTAINER_LOCATION_PREFIX PLAYER_HAND_LOCATION_SUBNAME

const char ITEM_GUID_KEYNAME[] = "global_id";
const char ITEM_NAME_KEYNAME[] = "name";
const char ITEM_DURABILITY_KEYNAME[] = "durability";
const char ITEM_STACK_SIZE_KEYNAME[] = "stack_size";
const char ITEM_LOCATION_KEYNAME[] = "location";
const char ITEM_LOCATION_ID_KEYNAME[] = "location_id";
const char ITEM_CONTAINER_SLOT_KEYNAME[] = "container_slot";

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

LocationNameID get_player_location_name_id(ItemContainerType container_type)
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

        default:
            GS_ASSERT(false);
            return LN_NONE;
    }
    GS_ASSERT(false);
    return LN_NONE;
}

int get_container_loaded_index_for_type(ItemContainerType type)
{
    switch (type)
    {
        case AGENT_HAND:
            return 0;
        case AGENT_INVENTORY:
            return 1;
        case AGENT_TOOLBELT:
            return 2;
        case AGENT_SYNTHESIZER:
            return 3;
        case AGENT_ENERGY_TANKS:
            return 4; 
        default:
            GS_ASSERT(false);
            return -1;
    }
    GS_ASSERT(false);
    return -1;
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


    // check that item containers for players are recognized, unique, within bounds
    int _cindex[N_PLAYER_CONTAINERS] = {-1};
    ItemContainerType _cindex_t[N_PLAYER_CONTAINERS] = {
        AGENT_HAND, AGENT_INVENTORY, AGENT_SYNTHESIZER,
        AGENT_ENERGY_TANKS, AGENT_TOOLBELT };

    // valid index
    for (int i=0; i<N_PLAYER_CONTAINERS; i++)
    {
        _cindex[i] = get_container_loaded_index_for_type(_cindex_t[i]);
        GS_ASSERT_ABORT(_cindex[i] >= 0 && _cindex[i] < N_PLAYER_CONTAINERS); 
    }

    // uniqueness
    for (int i=0; i<N_PLAYER_CONTAINERS-1; i++)
    for (int j=i+1; j<N_PLAYER_CONTAINERS; j++)
        GS_ASSERT_ABORT(_cindex[i] != _cindex[j]);
}

}   // serializer
