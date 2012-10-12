#pragma once

#if GS_SERIALIZER

namespace serializer
{

void init();
void teardown();
void update();

// defined in map.cpp
void load_map(const char* filename);
void save_map();
void wait_for_threads();

// defined in items.hpp
void save_player_container(int client_id, int container_id, bool remove_items_after);

int begin_player_load(UserID user_id, int client_id);
bool load_player_container(int player_load_id, int container_id); 
bool end_player_load(int player_load_id);

}   // serializer

#else

// stubs
namespace serializer
{

bool should_save_map = false;

void init(){}
void teardown(){}
void update();

void check_save_state(){}

void load_map(const char* filename){}
void save_map(){}
void wait_for_threads(){}

void save_player_container(int client_id, int container_id, bool remove_items_after) {}

int begin_player_load(UserID user_id, int client_id) { return 0; }
bool load_player_container(int player_load_id, int container_id) { return true; } 
bool end_player_load(int player_load_id) { return true; }

}   // serializer

#endif
