#pragma once

namespace t_map
{

void t_map_manager_setup(int client_id);
void t_map_manager_teardown(int client_id);

void t_map_manager_update(int client_id, int x, int y);
}