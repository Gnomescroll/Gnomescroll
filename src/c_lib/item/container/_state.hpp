#pragma once

namespace ItemContainer
{

// Common
extern class ItemContainerList* item_container_list;

// Client
#if DC_CLIENT
extern bool did_open_container_block;
extern bool did_close_container_block;
extern int opened_container;
extern int opened_container_event_id;

extern class ItemContainer* player_container;
extern class ItemContainer* player_toolbelt;
extern class ItemContainerHand* player_hand;
extern class ItemContainerSynthesizer* player_synthesizer;
extern class ItemContainerEnergyTanks* player_energy_tanks;
extern class ItemContainerCraftingBench* player_craft_bench;
extern class ItemContainer* storage_block;
extern class ItemContainerCryofreezer* cryofreezer;
extern class ItemContainerSmelter* smelter;
extern class ItemContainerCrusher* crusher;

extern class ItemContainerUI* player_container_ui;
extern class ItemContainerUI* player_toolbelt_ui;
extern class ItemContainerHandUI* player_hand_ui;
extern class ItemContainerSynthesizerUI* player_synthesizer_ui;
extern class ItemContainerEnergyTanksUI* player_energy_tanks_ui;
extern class ItemContainerUI* player_craft_bench_ui;
extern class ItemContainerUI* storage_block_ui;
extern class ItemContainerUI* cryofreezer_ui;
extern class ItemContainerSmelterUI* smelter_ui;
extern class ItemContainerCrusherUI* crusher_ui;
#endif

// Server
#if DC_SERVER
extern int* agent_container_list;
extern int* agent_toolbelt_list;
extern int* agent_synthesizer_list;
extern int* agent_energy_tanks_list;
extern int* agent_hand_list;
extern int* opened_containers;
#endif

}   // ItemContainer
