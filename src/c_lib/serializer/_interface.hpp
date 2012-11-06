#pragma once

#include <common/dirent.h>

typedef enum
{
    WORLD_SAVE_IDLE,
    WORLD_SAVE_UNFINISHED,
    WORLD_SAVE_FINISHED
} WorldSaveState;

namespace serializer
{

void init();
void teardown();
void update();

bool begin_new_world_version();

bool load_data();
bool save_data();

WorldSaveState update_save_state(int max_ms);

void wait_for_save_complete();

extern char map_folder[NAME_MAX+1];
extern char mech_folder[NAME_MAX+1];
extern char player_folder[NAME_MAX+1];
extern char container_folder[NAME_MAX+1];
extern char entity_folder[NAME_MAX+1];

extern char map_path[NAME_MAX+1];
extern char map_path_tmp[NAME_MAX+1];
extern char map_path_bak[NAME_MAX+1];
extern char map_palette_path[NAME_MAX+1];
extern char map_palette_path_tmp[NAME_MAX+1];
extern char map_palette_path_bak[NAME_MAX+1];
extern char mech_path[NAME_MAX+1];
extern char mech_path_tmp[NAME_MAX+1];
extern char mech_path_bak[NAME_MAX+1];
extern char mech_palette_path[NAME_MAX+1];
extern char mech_palette_path_tmp[NAME_MAX+1];
extern char mech_palette_path_bak[NAME_MAX+1];
extern char player_path[NAME_MAX+1];
extern char player_path_tmp[NAME_MAX+1];
extern char player_path_bak[NAME_MAX+1];
extern char container_path[NAME_MAX+1];
extern char container_path_tmp[NAME_MAX+1];
extern char container_path_bak[NAME_MAX+1];
extern char entity_path[NAME_MAX+1];
extern char entity_path_tmp[NAME_MAX+1];
extern char entity_path_bak[NAME_MAX+1];

}   // serializer

#if GS_SERIALIZER

namespace serializer
{

// defined in players.hpp
bool save_player(UserID user_id, AgentID agent_id);
bool save_player_container(ClientID client_id, int container_id);

int begin_player_load(UserID user_id, ClientID client_id);
bool load_player_container(int player_load_id, ItemContainerType container_type); 
bool end_player_load(int player_load_id);

// Called from outside this module. NetPeerManager is the only thing that knows about true failure
void player_load_failed();

bool create_player_container_items_from_data(AgentID agent_id, int* containers, int n_containers);

bool save_containers();

}   // serializer

#else

// stubs
namespace serializer
{

bool save_containers() { return true; }
void check_save_state() {}

bool save_player(UserID user_id, AgentID agent_id) { return true; }
bool save_player_container(ClientID client_id, int container_id) { return true; }

int begin_player_load(UserID user_id, ClientID client_id) { return 0; }
bool load_player_container(int player_load_id, ItemContainerType container_type) { return true; }
bool end_player_load(int player_load_id) { return true; }

void player_load_failed() {}

bool create_player_container_items_from_data(AgentID agent_id, int* containers, int n_containers) { return true; }

void wait_for_redis_replies() {}

}   // serializer

#endif
