#pragma once

#if DC_SERVER
# error Don't include this file in the server
#endif

namespace ItemContainer
{

const int CONTAINER_EVENT_MAX = 256;
extern ItemContainerID* container_event;

int record_container_event(ItemContainerID container_id);

void mouse_left_click_handler(ItemContainerID container_id, int slot, bool alt_action);
void mouse_right_click_handler(ItemContainerID container_id, int slot, bool alt_action);

void send_container_alpha_action(ContainerActionType action, ItemContainerID container_id, int slot);
void send_container_beta_action(ContainerActionType action, ItemContainerID container_id, int slot);
void send_synthesizer_alpha_action(ContainerActionType action, ItemContainerID container_id, int slot);
void send_synthesizer_beta_action(ContainerActionType action, ItemContainerID container_id, int slot);
void send_purchase_item_action(ContainerActionType action, ItemContainerID container_id, int slot);
void send_craft_alpha_action(ContainerActionType action, ItemContainerID container_id, int slot);
void send_craft_beta_action(ContainerActionType action, ItemContainerID container_id, int slot);
void send_craft_item_action(ContainerActionType action, ItemContainerID container_id, int slot);
void send_no_container_alpha_action(ContainerActionType action, ItemContainerID container_id, int slot);
void send_no_container_beta_action(ContainerActionType action, ItemContainerID container_id, int slot);
void send_smelter_alpha_action(ContainerActionType action, ItemContainerID container_id, int slot);
void send_smelter_beta_action(ContainerActionType action, ItemContainerID container_id, int slot);
void send_crusher_alpha_action(ContainerActionType action, ItemContainerID container_id, int slot);
void send_crusher_beta_action(ContainerActionType action, ItemContainerID container_id, int slot);
void send_crusher_crush_action(ContainerActionType action, ItemContainerID container_id, int slot);

void send_container_open(ItemContainerID container_id, int event_id);
void send_container_close(ItemContainerID container_id);

}   // ItemContainer
