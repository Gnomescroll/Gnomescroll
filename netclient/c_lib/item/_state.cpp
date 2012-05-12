#include "_state.hpp"

namespace Item
{

//Common
ItemContainerList* item_container_list = NULL;
ItemList* item_list = NULL;


//Client
#if DC_CLIENT

int player_container_id = NULL_CONTAINER;   //store id of player container
int player_toolbelt_id = NULL_CONTAINER;   //store id of player container
int player_nanite_id = NULL_CONTAINER;   //store id of player container
int player_craft_bench_id = NULL_CONTAINER;

class ItemContainer* player_container = NULL;
class ItemContainer* player_toolbelt = NULL;
class ItemContainerNanite* player_nanite = NULL;
class ItemContainerCraftingBench* player_craft_bench = NULL;
ItemID player_hand = NULL_ITEM;

class ItemContainerUI* player_container_ui = NULL;
class ItemContainerUI* player_toolbelt_ui = NULL;
class ItemContainerNaniteUI* player_nanite_ui = NULL;
class ItemContainerUI* player_craft_bench_ui = NULL;
int player_hand_type_ui = NULL_ITEM_TYPE;
int player_hand_stack_ui = 1;
int player_hand_durability_ui = NULL_DURABILITY;
#endif


//Server
#if DC_SERVER

int* agent_container_list = NULL;
int* agent_toolbelt_list = NULL;
int* agent_nanite_list = NULL;
int* agent_craft_bench_list = NULL;
ItemID* agent_hand_list = NULL;

#endif

}
