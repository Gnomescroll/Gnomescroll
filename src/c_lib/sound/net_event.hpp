#pragma once

namespace Sound
{

class sound_event_StoC: public FixedSizeReliableNetPacketToClient<sound_event_StoC>
{
    public:

        float x,y,z;
        float mx,my,mz; // send initial impulse, not velocity
        uint8_t owner;
        uint16_t ttl_max;
        uint8_t type;

        inline void packet(char* buff, unsigned int* buff_n, bool pack) 
        {
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);

            pack_float(&mx, buff, buff_n, pack);
            pack_float(&my, buff, buff_n, pack);
            pack_float(&mz, buff, buff_n, pack);

            pack_u8(&owner, buff, buff_n, pack);
            pack_u16(&ttl_max, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
        }
        inline void handle();
};

inline void grenade_StoC::handle()
{
    #if DC_CLIENT
    if (owner == ClientState::playerAgent_state.agent_id) return;
    Grenade* g = Particle::grenade_list->create();
    if (g == NULL) return;
    g->set_state(x, y, z, mx, my, mz);
    g->owner = owner;
    g->ttl_max = (int)ttl_max;
    g->type = (int)type;
    #endif
}

}