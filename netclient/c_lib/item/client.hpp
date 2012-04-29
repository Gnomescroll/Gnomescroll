#pragma once

#if DC_CLIENT

namespace Item
{

const int CONTAINER_EVENT_MAX = 256;
extern int container_event[CONTAINER_EVENT_MAX];

void mouse_left_click_handler(int id, int slot);
void mouse_right_click_handler(int id, int slot);

}

#endif
