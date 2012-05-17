#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

namespace ItemContainer
{

const int CONTAINER_EVENT_MAX = 256;
extern int container_event[CONTAINER_EVENT_MAX];

int record_container_event(int container_id);

void mouse_left_click_handler(int container_id, int slot, bool nanite, bool craft_output);
void mouse_right_click_handler(int container_id, int slot, bool nanite, bool craft_output);

}   // ItemContainer
