/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

namespace Toolbelt
{

// Common
extern bool* agent_fire_on;
extern int* agent_fire_cooldown;
extern ItemType* agent_selected_type;
extern ItemType fist_item_type;    // cached item type lookup

// Client
#if DC_CLIENT
extern int selected_slot;
extern ItemContainerID toolbelt_id;
extern bool left_down;      // indicates whether the left button is down
                            // this is solely to manage erroneous left_up events
                            // from SDL that happen when unlocking the mouse.
                            // do not use it for any other purpose.
extern bool single_trigger; // last event was a single trigger event
extern bool holding;        // is holding button down (Note: this doesn't
                            // mean literally. Single trigger charge_never
                            // events will cancel 'holding')
extern int holding_tick;    // how long we've been holding down
extern bool charging;       // player is charging their weapon. it is different
                            // from holding in that the player is only considered
                            // charging if the weapon supports charging and
                            // they've been holding longer than some threshold
#endif

// Server
#if DC_SERVER
extern int* agent_selected_slot;
extern ItemID* agent_selected_item;
#endif

void init_state();
void teardown_state();

}    // Toolbelt
