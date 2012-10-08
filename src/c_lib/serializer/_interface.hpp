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
void load_player_container(int client_id, int container_id);

}   // serializer

#else

// stubs
namespace serializer
{

bool should_save_map = false;

void init(){}
void teardown(){}

void check_save_state(){}

void load_map(const char* filename){}
void save_map(){}
void wait_for_threads(){}

void save_player_container(int client_id, int container_id, bool remove_items_after) {}
void load_player_container(int client_id, int container_id) {}

}   // serializer

#endif
