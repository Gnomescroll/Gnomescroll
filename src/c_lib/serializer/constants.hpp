#pragma once

// paths

#define DATA_PATH "./world/"

#define INVALID_DATA_SUBPATH "invalid/"

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
    SE_SAVE_ITEM_ITEM_NAME_NOT_FOUND,
    SE_SAVE_ITEM_FILENAME_LONG,
    SE_SAVE_ITEM_NULL_FILE,
    SE_SAVE_ITEM_WRITE_DATA_FAILED,
    SE_SAVE_ITEM_INVALID_GLOBAL_ID,
    SE_SAVE_ITEM_INVALID_ITEM_NAME,
    SE_SAVE_ITEM_INVALID_LOCATION_DATA,
    SE_SAVE_ITEM_INVALID_DURABILITY,
    SE_SAVE_ITEM_INVALID_STACK_SIZE,

    // item loading
    SE_LOAD_ITEM_FILENAME_LONG,
    SE_LOAD_ITEM_FILE_NULL,
    SE_LOAD_ITEM_DATA_NOT_FOUND,
    SE_LOAD_ITEM_CREATE_ITEM_FAILED,
    SE_LOAD_ITEM_NAME_UNRECOGNIZED,
    // these are different from SE_PARSE_ITEM_* of the same name, in that they are
    // checked based on realistic values for items, and not just ascii parseability
    SE_LOAD_ITEM_INVALID_GLOBAL_ID,
    SE_LOAD_ITEM_INVALID_LOCATION,
    SE_LOAD_ITEM_INVALID_DURABILITY,
    SE_LOAD_ITEM_INVALID_STACK_SIZE,
    SE_LOAD_ITEM_INVALID_NAME,

    // parsing item file
    SE_PARSE_ITEM_INVALID_TOKEN_LENGTH,
    SE_PARSE_ITEM_UNRECOGNIZED_TOKEN,
    SE_PARSE_ITEM_BAD_PIECES_COUNT,
    SE_PARSE_ITEM_IMPOSSIBLE_TOKEN_MATCH,
    // these are invalid if they are out of basic integer ranges or the data was
    // invalid from an ascii perspective
    SE_PARSE_ITEM_INVALID_GLOBAL_ID,
    SE_PARSE_ITEM_INVALID_LOCATION,
    SE_PARSE_ITEM_INVALID_LOCATION_ID,
    SE_PARSE_ITEM_INVALID_CONTAINER_SLOT,
    SE_PARSE_ITEM_INVALID_DURABILITY,
    SE_PARSE_ITEM_INVALID_STACK_SIZE,
    
    // load item global id
    SE_LOAD_ITEM_GLOBAL_ID_FILE_ERROR,
    SE_LOAD_ITEM_GLOBAL_ID_INVALID,
    
    // write item global id
    SE_WRITE_ITEM_GLOBAL_ID_FILE_ERROR,
    
} SerializerError;
