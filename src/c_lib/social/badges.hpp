#pragma once

#include <SDL/constants.hpp>
#include <net_lib/net_StoC.hpp>

const size_t PLAYER_MAX_BADGES = 5;

const size_t BADGE_NAME_MAX_LENGTH = DAT_NAME_MAX_LENGTH;
const size_t MAX_BADGES = 64;

typedef enum
{
    NULL_BADGE = MAX_BADGES
} BadgeID;

inline bool isValid(BadgeID id)
{
    return (id >= 0 && id < (BadgeID)MAX_BADGES);
}

namespace Badges
{

class Badge
{
    public:
        bool loaded;

        BadgeID id;
        char name[BADGE_NAME_MAX_LENGTH+1];
        int sprite_id;

    Badge() :
        loaded(false), id(NULL_BADGE), sprite_id(NULL_SPRITE)
    {
        memset(name, 0, sizeof(this->name));
    }
};

extern size_t n_badges;
extern class Badge badges[MAX_BADGES];

void register_badges();

BadgeID get_badge(const char* name);

int get_badge_sprite(BadgeID id);
const char* get_badge_name(BadgeID id);

void init_packets();

#if DC_SERVER
void broadcast_badge(BadgeID badge_id, AgentID agent_id);
void send_badge(BadgeID badge_id, AgentID agent_id, ClientID client_id);
#endif

class add_badge_StoC: public FixedSizeReliableNetPacketToClient<add_badge_StoC>
{
    public:
        uint8_t badge_id;
        uint8_t agent_id;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u8(&badge_id, buff, buff_n, pack);
        pack_u8(&agent_id, buff, buff_n, pack);
    }

    inline void handle();
};

}   // Badges
