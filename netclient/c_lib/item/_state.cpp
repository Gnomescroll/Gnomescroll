#include "_state.hpp"

namespace item
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

#endif


//Server
#if DC_SERVER

int AgentInventoryList[256];
int AgentToolbeltList[256];
int AgentNaniteList[256];
int AgentItemInHand[256];

#endif

}