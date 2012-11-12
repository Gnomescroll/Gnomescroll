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
void log_mech_load_error(const char* msg, int x, int y, int z, MechType mech_type, int subtype);

#if GS_SERIALIZER
void log_container_save_error(const char* msg);
void log_container_load_error(const char* msg, const char* data_str,
    class ParsedContainerFileData* file_data, class ParsedContainerData* container_data,
    class ParsedItemData* item_data);
void log_container_load_error(const char* msg)
{
    log_container_load_error(msg, NULL, NULL, NULL, NULL);
}

void log_player_save_error(const char* msg);
void log_player_save_error(const char* msg,
    UserID user_id, AgentID agent_id,
    class ItemContainer::ItemContainerInterface* container);

void log_player_load_error(const char* msg);
void log_player_load_error(const char* msg, const char* data_str,
    class PlayerLoadData* load_data, class PlayerContainerLoadData* container_load_data,
    class ParsedPlayerData* player_data, class ParsedPlayerContainerData* container_data,
    class ParsedItemData* item_data);

void log_entity_save_error(const char* msg, const char* entity_name, int entity_id);
void log_entity_save_error(const char* msg);
void log_entity_load_error(const char* msg);
#endif

}   // serializer
