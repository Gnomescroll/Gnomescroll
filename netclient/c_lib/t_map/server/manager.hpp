#pragma once

namespace t_map
{

extern class Terrain_map_history* map_history;

void t_map_manager_setup(int client_id);
void t_map_manager_teardown(int client_id);

void t_map_manager_update_client_position(int client_id, float _x, float _y);

void t_map_manager_update();
void t_map_manager_force_update(int client_id);

}   // t_map
