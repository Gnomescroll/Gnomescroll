#pragma once

namespace Toolbelt
{

void init_config();
void teardown_config();

/* Getters */

ClickAndHoldBehaviour get_item_click_and_hold_behaviour(ItemType item_type);
ChargeBehaviour get_item_charge_behaviour(ItemType item_type);

/* Callback getters */

tickItem get_tick_item_fn(ItemType item_type);
triggerItem get_trigger_item_fn(ItemType item_type);
triggerItem get_trigger_item_beta_fn(ItemType item_type);

#if DC_CLIENT
tickLocalItem get_tick_local_item_fn(ItemType item_type);
triggerLocalItem get_trigger_local_item_fn(ItemType item_type);
triggerLocalItem get_trigger_local_item_beta_fn(ItemType item_type);
beginLocalItem get_begin_local_item_fn(ItemType item_type);
endLocalItem get_end_local_item_fn(ItemType item_type);
#endif


}    // Toolbelt
