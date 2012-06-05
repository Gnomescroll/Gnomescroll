#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

namespace Item
{

// packets
void broadcast_item_create(ItemID item_id);
class Item* send_item_create(int client_id, ItemID item_id);

void send_item_state(int client_id, ItemID item_id);
void broadcast_item_state(ItemID item_id);

void send_item_destroy(int client_id, ItemID item_id);
void broadcast_item_destroy(ItemID item_id);

}   // ItemContainer
