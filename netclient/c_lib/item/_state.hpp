#pragma once

namespace item
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

extern int AgentInventoryList[256];
extern int AgentToolbeltList[256];
extern int AgentNaniteList[256];
extern int AgentItemInHand[256];

#endif

}