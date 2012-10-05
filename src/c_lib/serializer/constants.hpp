#pragma once

#include <item/container/config/_state.hpp>

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

// DONT CHANGE THESE
#define CONTAINER_LOCATION_NAME "container"
#define PARTICLE_LOCATION_NAME "particle"
#define PLAYER_INVENTORY_LOCATION_NAME "player:inventory"
#define PLAYER_ENERGY_TANKS_LOCATION_NAME "player:energy_tanks"
#define PLAYER_SYNTHESIZER_LOCATION_NAME "player:synthesizer"
#define PLAYER_TOOLBELT_LOCATION_NAME "player:toolbelt"
#define PLAYER_HAND_LOCATION_NAME "player:hand"

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

        case AGENT_CONTAINER:
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

    bool agent_container_found = false;
    bool agent_toolbelt_found = false;
    bool agent_synthesizer_found = false;
    bool agent_energy_tanks_found = false;
    bool agent_hand_found = false;

    // make sure all container types are loaded
    
    for (int i=0; i<MAX_CONTAINER_TYPES; i++)
    {
        ContainerAttributes* attr = &ItemContainer::container_attributes[i];
        if (!attr->loaded || !attr->attached_to_agent) continue;
        switch (attr->type)
        {
            case AGENT_CONTAINER:
                agent_container_found = true;
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

    GS_ASSERT_ABORT(agent_container_found);
    GS_ASSERT_ABORT(agent_toolbelt_found);
    GS_ASSERT_ABORT(agent_synthesizer_found);
    GS_ASSERT_ABORT(agent_energy_tanks_found);
    GS_ASSERT_ABORT(agent_hand_found);
}
