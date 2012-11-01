#pragma once

namespace serializer
{

void init_logger();
void teardown_logger();
void set_log_paths(const char* save_folder);

void log_map_save_error(const char* msg);
void log_map_load_error(const char* msg);

void log_mech_save_error(const char* msg);
void log_mech_load_error(const char* msg);

#if GS_SERIALIZER
void log_container_save_error(const char* msg);
void log_container_load_error(const char* msg,
    class ParsedContainerFileData* file_data, class ParsedContainerData* container_data,
    class ParsedItemData* item_data);

void log_player_save_error(const char* msg);
void log_player_load_error(const char* msg,
    class PlayerLoadData* load_data, class PlayerContainerLoadData* container_load_data,
    class ParsedPlayerData* player_data, class ParsedPlayerContainerData* container_data,
    class ParsedItemData* item_data);
#endif

}   // serializer
