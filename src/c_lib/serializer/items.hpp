#pragma once

#include <common/template/elastic_object_list.hpp>
#include <serializer/constants.hpp>
#include <net_lib/server.hpp>

namespace serializer
{

void init_items();
void teardown_items();

bool save_player(UserID user_id, AgentID agent_id);
bool save_player_container(ClientID client_id, int container_id);

int begin_player_load(UserID user_id, ClientID client_id, AgentID agent_id);
bool load_player_container(int player_load_id, int container_id); 
bool end_player_load(int player_load_id);

}   // serializer
