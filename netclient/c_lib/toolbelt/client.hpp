#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

namespace Toolbelt
{

void send_set_slot_packet(int slot)
{
    assert(slot >= 0 && slot != NULL_SLOT && slot < TOOLBELT_MAX_SLOTS);
    toolbelt_set_slot_CtoS msg;
    msg.slot = slot;
    msg.send();
}

} // Toolbelt

