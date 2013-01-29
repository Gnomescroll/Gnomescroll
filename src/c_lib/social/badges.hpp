#pragma once

#include <SDL/constants.hpp>
#include <net_lib/net_StoC.hpp>

const size_t PLAYER_MAX_BADGES = 5;
const size_t MAX_BADGES = 64;

typedef enum
{
    NULL_BADGE = MAX_BADGES
} BadgeType;

inline bool isValid(BadgeType type)
{
    return (type >= 0 && type < (BadgeType)MAX_BADGES);
}

namespace Badges
{

void register_badges();

int get_badge_sprite(BadgeType type);

BadgeType get_badge_type(const char* name);
const char* get_badge_name(BadgeType type);

void init();
void teardown();

void init_packets();

#if DC_SERVER
void broadcast_badge(BadgeType badge_type, AgentID agent_id);
void send_badge(BadgeType badge_type, AgentID agent_id, ClientID client_id);
#endif

class add_badge_StoC: public FixedSizeReliableNetPacketToClient<add_badge_StoC>
{
    public:
        uint8_t badge_type;
        uint8_t agent_id;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u8(&badge_type, buff, buff_n, pack);
        pack_u8(&agent_id, buff, buff_n, pack);
    }

    inline void handle();
};

}   // Badges
