#include "_state.hpp"

namespace Item
{

//Common
ItemContainerList* item_container_list = NULL;
ItemList* item_list = NULL;


//Client
#if DC_CLIENT

int player_inventory_id = -1;   //store id of player inventory
int player_toolbelt_id = -1;   //store id of player inventory
int player_nanite_id = -1;   //store id of player inventory

class ItemContainer* player_inventory = NULL;
class ItemContainer* player_toolbelt = NULL;
class ItemContainer* player_nanite = NULL;
ItemID player_hand = NULL_ITEM;

#endif


//Server
#if DC_SERVER

int* agent_inventory_list = NULL;
int* agent_toolbelt_list = NULL;
int* agent_nanite_list = NULL;
int* agent_hand_list = NULL;

#endif

}
