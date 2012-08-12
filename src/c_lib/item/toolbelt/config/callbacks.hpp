#pragma once

namespace Toolbelt
{

#if DC_CLIENT

// use as a non-null placeholder
void local_trigger_dummy(ItemID item_id, int item_type) {}

void tick_mining_laser(int agent_id, int item_type);
void trigger_mining_laser(int agent_id, int item_type);

void begin_mining_laser(int item_type);
void end_mining_laser(int item_type);

void tick_local_mining_laser(ItemID item_id, int item_type);
void trigger_local_mining_laser(ItemID item_id, int item_type);

void trigger_local_block_placer(ItemID item_id, int item_type);
void trigger_local_location_pointer(ItemID item_id, int item_type);
void trigger_local_admin_block_placer(ItemID item_id, int item_type);
void trigger_local_plasma_grenade(ItemID item_id, int item_type);
void trigger_local_hitscan_laser(ItemID item_id, int item_type);;
void fire_close_range_weapon(ItemID item_id, int item_type);
#endif

#if DC_SERVER
void decrement_durability(int agent_id, ItemID item_id, int item_type);
void decrement_stack(int agent_id, ItemID item_id, int item_type);
void consume_item(int agent_id, ItemID item_id, int item_type);
void apply_charge_pack_to_teammates(int agent_id, ItemID item_id, int item_type);
void place_spawner(int agent_id, ItemID item_id, int item_type);
void place_energy_core(int agent_id, ItemID item_id, int item_type);
void place_mech(int agent_id, ItemID item_id, int item_type);
#endif

}    // Toolbelt
