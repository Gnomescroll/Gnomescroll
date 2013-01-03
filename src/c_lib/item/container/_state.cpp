#include "_state.hpp"

namespace ItemContainer
{

//Common
ItemContainerList* item_container_list = NULL;

//Client
#if DC_CLIENT
bool did_open_container_block = false;
bool did_close_container_block = false;
ItemContainerID opened_container = NULL_CONTAINER;
int opened_container_event_id = 0;

class ItemContainer* player_container = NULL;
class ItemContainer* player_toolbelt = NULL;
class ItemContainerHand* player_hand = NULL;
class ItemContainerSynthesizer* player_synthesizer = NULL;
class ItemContainerEnergyTanks* player_energy_tanks = NULL;
class ItemContainerCraftingBench* player_craft_bench = NULL;
class ItemContainer* storage_block = NULL;
class ItemContainerCryofreezer* cryofreezer = NULL;
class ItemContainerSmelter* smelter = NULL;
class ItemContainerCrusher* crusher = NULL;

class ItemContainerUI* player_container_ui = NULL;
class ItemContainerUI* player_toolbelt_ui = NULL;
class ItemContainerHandUI* player_hand_ui = NULL;
class ItemContainerSynthesizerUI* player_synthesizer_ui = NULL;
class ItemContainerEnergyTanksUI* player_energy_tanks_ui;
class ItemContainerUI* player_craft_bench_ui = NULL;
class ItemContainerUI* storage_block_ui = NULL;
class ItemContainerUI* cryofreezer_ui = NULL;
class ItemContainerSmelterUI* smelter_ui = NULL;
class ItemContainerCrusherUI* crusher_ui = NULL;
#endif


//Server
#if DC_SERVER
ItemContainerID* agent_inventory_list = NULL;
ItemContainerID* agent_toolbelt_list = NULL;
ItemContainerID* agent_synthesizer_list = NULL;
ItemContainerID* agent_energy_tanks_list = NULL;
ItemContainerID* agent_hand_list = NULL;
ItemContainerID* opened_containers = NULL;
#endif

}   // ItemContainer
