#pragma once

// paths

#define DATA_PATH "./world/"

#define MAP_DATA_PATH       DATA_PATH "map/"
#define MECH_DATA_PATH      DATA_PATH "mechs/"
#define PLAYER_DATA_PATH    DATA_PATH "players/"
#define CONTAINER_DATA_PATH DATA_PATH "containers/"
#define ITEM_DATA_PATH      DATA_PATH "items/"

#define MAP_DATA_EXT       ".map"
#define MECH_DATA_EXT      ".mch"
#define PLAYER_DATA_EXT    ".ply"
#define CONTAINER_DATA_EXT ".ctr"
#define ITEM_DATA_EXT      ".itm"

const char default_map_file[] = MAP_DATA_PATH "map-" STR(GS_VERSION) MAP_DATA_EXT;
const char mech_filename[] = MECH_DATA_PATH "mech-" STR(GS_VERSION) MECH_DATA_EXT;

const char item_filename_fmt[] = ITEM_DATA_PATH "item-%010d" ITEM_DATA_EXT;
const char container_filename_fmt[] = CONTAINER_DATA_PATH "container-%010d" CONTAINER_DATA_EXT;
const char player_filename_fmt[] = PLAYER_DATA_PATH "%s" PLAYER_DATA_EXT;

const char item_global_id_filename[] = ITEM_DATA_PATH "global.id";

typedef enum
{
    SE_NONE = 0,

    // item saving
    SE_SAVE_ITEM_ITEM_NOT_FOUND,
    SE_SAVE_ITEM_ITEM_ID_MISMATCH,
    SE_SAVE_ITEM_FILENAME_LONG,
    SE_SAVE_ITEM_NULL_FILE,

    // item loading
    SE_LOAD_ITEM_DATA_NOT_FOUND,
    SE_LOAD_ITEM_CREATE_ITEM_FAILED,

    // load item global id
    SE_LOAD_ITEM_GLOBAL_ID_FILE_ERROR,
    SE_LOAD_ITEM_GLOBAL_ID_INVALID,

    // write item global id
    SE_WRITE_ITEM_GLOBAL_ID_FILE_ERROR,
    
} SerializerError;
