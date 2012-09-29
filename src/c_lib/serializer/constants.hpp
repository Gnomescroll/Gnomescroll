#pragma once

// paths

#define DATA_PATH "./world/"

#define MAP_DATA_PATH       DATA_PATH "map/"
#define MECH_DATA_PATH      DATA_PATH "mechs/"
#define PLAYER_DATA_PATH    DATA_PATH "players/"
#define CONTAINER_DATA_PATH DATA_PATH "containers/"
#define ITEM_DATA_PATH      DATA_PATH "items/"


// Map data
const char default_map_file[] = MAP_DATA_PATH "map-" STR(GS_VERSION) ".map";
