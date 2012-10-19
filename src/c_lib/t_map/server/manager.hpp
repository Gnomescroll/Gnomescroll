#pragma once

namespace t_map
{

extern class Terrain_map_subscription* map_history;

void t_map_manager_setup(ClientID client_id);
void t_map_manager_teardown(ClientID client_id);

void t_map_manager_update_client_position(ClientID client_id, float _x, float _y);

void t_map_manager_update();
void t_map_manager_force_update(ClientID client_id);

}   // t_map
