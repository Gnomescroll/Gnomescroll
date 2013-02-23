#pragma once

namespace Toolbelt
{

#if DC_CLIENT

// use as a non-null placeholder
void local_trigger_dummy(ItemID item_id, ItemType item_type) {}

void begin_local_mining_laser(ItemType item_type);
void end_local_mining_laser(ItemType item_type);
void begin_mining_laser(AgentID agent_id, ItemType item_type);
void end_mining_laser(AgentID agent_id, ItemType item_type);
void trigger_local_mining_laser(ItemID item_id, ItemType item_type);

void trigger_local_block_placer(ItemID item_id, ItemType item_type);
void trigger_local_location_pointer(ItemID item_id, ItemType item_type);
void trigger_local_beta_location_pointer(ItemID item_id, ItemType item_type);
void trigger_local_admin_block_placer(ItemID item_id, ItemType item_type);
void trigger_local_plasma_grenade(ItemID item_id, ItemType item_type);
void trigger_local_hitscan_laser(ItemID item_id, ItemType item_type);
void fire_close_range_weapon(ItemID item_id, ItemType item_type);

void select_facing_block(ItemID item_id, ItemType item_type);
#endif

#if DC_SERVER
void decrement_durability(AgentID agent_id, ItemID item_id, ItemType item_type);
void decrement_stack(AgentID agent_id, ItemID item_id, ItemType item_type);
void consume_item(AgentID agent_id, ItemID item_id, ItemType item_type);
void apply_charge_pack_to_teammates(AgentID agent_id, ItemID item_id, ItemType item_type);
void place_spawner(AgentID agent_id, ItemID item_id, ItemType item_type);
void place_energy_core(AgentID agent_id, ItemID item_id, ItemType item_type);
void place_mech(AgentID agent_id, ItemID item_id, ItemType item_type);
void use_boon_crank(AgentID agent_id, ItemID item_id, ItemType item_type);
void plant_placer_action(AgentID agent_id, ItemID item_id, ItemType item_type);
#endif

}    // Toolbelt
