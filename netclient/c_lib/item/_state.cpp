#include "_state.hpp"

namespace Item
{

//Common
ItemContainerList* item_container_list = NULL;
ItemList* item_list = NULL;


//Client
#if DC_CLIENT

int player_container_id = -1;   //store id of player container
int player_toolbelt_id = -1;   //store id of player container
int player_nanite_id = -1;   //store id of player container

class ItemContainer* player_container = NULL;
class ItemContainer* player_toolbelt = NULL;
class ItemContainer* player_nanite = NULL;
ItemID player_hand = NULL_ITEM;

class ItemContainerUI* player_container_ui = NULL;
class ItemContainerUI* player_toolbelt_ui = NULL;
class ItemContainerUI* player_nanite_ui = NULL;
int player_hand_type_ui = NULL_ITEM_TYPE;
int player_hand_stack_ui = 1;
int player_hand_durability_ui = NULL_DURABILITY;
#endif


//Server
#if DC_SERVER

int* agent_container_list = NULL;
int* agent_toolbelt_list = NULL;
int* agent_nanite_list = NULL;
ItemID* agent_hand_list = NULL;

#endif

}
