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
extern int player_craft_bench_id;

extern class ItemContainer* player_container;
extern class ItemContainer* player_toolbelt;
extern class ItemContainerNanite* player_nanite;
extern class ItemContainerCraftingBench* player_craft_bench;
extern ItemID player_hand;

extern class ItemContainerUI* player_container_ui;
extern class ItemContainerUI* player_toolbelt_ui;
extern class ItemContainerNaniteUI* player_nanite_ui;
extern class ItemContainerUI* player_craft_bench_ui;
extern int player_hand_type_ui;
extern int player_hand_stack_ui;
extern int player_hand_durability_ui;
#endif

// Server
#if DC_SERVER

extern int* agent_container_list;
extern int* agent_toolbelt_list;
extern int* agent_nanite_list;
extern int* agent_craft_bench_list;
extern ItemID* agent_hand_list;

#endif

}
