#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

namespace Toolbelt
{

bool toolbelt_item_begin_alpha_action();
bool toolbelt_item_end_alpha_action();
bool toolbelt_item_beta_action();
bool toolbelt_item_reload_action();

void send_set_slot_packet(int slot)
{
    GS_ASSERT(slot >= 0 && slot != NULL_SLOT && slot < TOOLBELT_MAX_SLOTS);
    toolbelt_set_slot_CtoS msg;
    msg.slot = slot;
    msg.send();
}

void send_begin_alpha_action_packet()
{
    toolbelt_begin_alpha_action_CtoS msg;
    msg.send();
}

void send_end_alpha_action_packet()
{
    toolbelt_end_alpha_action_CtoS msg;
    msg.send();
}

void send_beta_action_packet()
{
    toolbelt_beta_action_CtoS msg;
    msg.send();
}

void send_reload_action_packet()
{
    toolbelt_reload_action_CtoS msg;
    msg.send();
}

} // Toolbelt

