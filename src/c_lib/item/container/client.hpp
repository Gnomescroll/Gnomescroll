#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

namespace ItemContainer
{

const int CONTAINER_EVENT_MAX = 256;
extern int* container_event;

int record_container_event(int container_id);

void mouse_left_click_handler(int container_id, int slot, bool alt_action);
void mouse_right_click_handler(int container_id, int slot, bool alt_action);

void send_container_alpha_action(ContainerActionType action, int container_id, int slot);
void send_container_beta_action(ContainerActionType action, int container_id, int slot);
void send_synthesizer_alpha_action(ContainerActionType action, int container_id, int slot);
void send_synthesizer_beta_action(ContainerActionType action, int container_id, int slot);
void send_purchase_item_action(ContainerActionType action, int container_id, int slot);
void send_craft_alpha_action(ContainerActionType action, int container_id, int slot);
void send_craft_beta_action(ContainerActionType action, int container_id, int slot);
void send_craft_item_action(ContainerActionType action, int container_id, int slot);
void send_no_container_alpha_action(ContainerActionType action, int container_id, int slot);
void send_no_container_beta_action(ContainerActionType action, int container_id, int slot);
void send_smelter_alpha_action(ContainerActionType action, int container_id, int slot);
void send_smelter_beta_action(ContainerActionType action, int container_id, int slot);
void send_recycler_alpha_action(ContainerActionType action, int container_id, int slot);
void send_recycler_beta_action(ContainerActionType action, int container_id, int slot);
void send_recycler_crush_action(ContainerActionType action, int container_id, int slot);

void send_container_close(int container_id);

}   // ItemContainer
