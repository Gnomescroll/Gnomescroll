#pragma once

#if DC_SERVER

namespace Item
{

void send_container_failed_action(int client_id, int event_id)
{
    container_action_failed_StoC msg;
    msg.event_id = event_id;
    msg.sendToClient(client_id);
}


}

#endif
