#pragma once

namespace Item
{

// Common
extern class ItemContainerList* item_container_list;
extern class ItemList* item_list;


// Client
#if DC_CLIENT

extern int player_container_id;
extern int player_toolbelt_id;
extern int player_nanite_id;

extern class ItemContainer* player_container;
extern class ItemContainer* player_toolbelt;
extern class ItemContainer* player_nanite;
extern class ItemContainerUI* player_container_ui;
extern class ItemContainerUI* player_toolbelt_ui;
extern class ItemContainerUI* player_nanite_ui;
extern ItemID player_hand;
#endif

// Server
#if DC_SERVER

extern int* agent_container_list;
extern int* agent_toolbelt_list;
extern int* agent_nanite_list;
extern ItemID* agent_hand_list;

#endif

}
