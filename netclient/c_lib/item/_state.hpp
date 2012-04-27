#pragma once

namespace Item
{

// Common
extern class ItemContainerList* item_container_list;
extern class ItemList* item_list;


// Client
#if DC_CLIENT

extern int player_inventory_id;
extern int player_toolbelt_id;
extern int player_nanite_id;

extern class ItemContainer* player_inventory;
extern class ItemContainer* player_toolbelt;
extern class ItemContainer* player_nanite;

#endif

// Server
#if DC_SERVER

extern int* agent_inventory_list;
extern int* agent_toolbelt_list;
extern int* agent_nanite_list;
extern int* agent_hand_list;

#endif

}
