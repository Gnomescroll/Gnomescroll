#include "badges.hpp"

#include <SDL/texture_sheet_loader.hpp>
#include <common/common.hpp>
#include <agent/_interface.hpp>

namespace Badges
{

size_t n_badges = 0;
class Badge badges[MAX_BADGES];

static void add_badge(const char* name, SpriteSheet sheet_id, int xpos, int ypos)
{
    IF_ASSERT(n_badges >= MAX_BADGES) return;
    
    GS_ASSERT(name[0] != '\0');
    GS_ASSERT(is_valid_name(name));
    GS_ASSERT(sheet_id != NULL_SPRITE_SHEET);
    GS_ASSERT(xpos > 0 && ypos > 0);

    badges[n_badges].id = (BadgeID)n_badges;
    badges[n_badges].loaded = true;

    strncpy(badges[n_badges].name, name, BADGE_NAME_MAX_LENGTH+1);
    badges[n_badges].name[BADGE_NAME_MAX_LENGTH] = '\0';

    #if DC_CLIENT
    int sprite_id = TextureSheetLoader::badge_texture_sheet_loader->blit(sheet_id, xpos, ypos);
    GS_ASSERT(sprite_id != NULL_SPRITE);
    badges[n_badges].sprite_id = sprite_id;
    #endif

    n_badges++;
}

static void verify_badge_conf()
{
    if (!n_badges) return;
    
    // check valid values
    for (size_t i=0; i<n_badges; i++)
    {
        GS_ASSERT(badges[i].name[0] != '\0');
        GS_ASSERT(is_valid_name(badges[i].name));
        #if DC_CLIENT
        GS_ASSERT(badges[i].sprite_id != NULL_SPRITE);
        #endif
    }

    // check configs against each other for duplicates
    for (size_t i=0; i<n_badges-1; i++)
    for (size_t j=i+1; j<n_badges; j++)
    {
        GS_ASSERT(strcmp(badges[i].name, badges[j].name) != 0);
        #if DC_CLIENT
        GS_ASSERT(badges[i].sprite_id != badges[j].sprite_id);
        #endif
    }
}

BadgeID get_badge(const char* name)
{
    IF_ASSERT(name[0] == '\0') return NULL_BADGE;
    for (size_t i=0; i<MAX_BADGES; i++)
        if (strcmp(badges[i].name, name) == 0)
            return badges[i].id;
    GS_ASSERT(false);
    printf("No badge found for name: %s\n", name);
    return NULL_BADGE;
}

int get_badge_sprite(BadgeID id)
{
    IF_ASSERT(!isValid(id)) return NULL_SPRITE;
    IF_ASSERT(!badges[id].loaded) return NULL_SPRITE;
    return badges[id].sprite_id;
}

const char* get_badge_name(BadgeID id)
{
    IF_ASSERT(!isValid(id)) return NULL;
    IF_ASSERT(!badges[id].loaded) return NULL;
    return badges[id].name;
}

void init_packets()
{
    add_badge_StoC::register_client_packet();
}

#if DC_SERVER
static bool prep_add_badge(add_badge_StoC* msg, BadgeID badge_id, AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return false;
    IF_ASSERT(!isValid(badge_id)) return false;
    
    msg->badge_id = badge_id;
    msg->agent_id = agent_id;
    return true;
}

void broadcast_badge(BadgeID badge_id, AgentID agent_id)
{    
    add_badge_StoC msg;
    if (!prep_add_badge(&msg, badge_id, agent_id)) return;
    msg.broadcast();
}

void send_badge(BadgeID badge_id, AgentID agent_id, ClientID client_id)
{
    IF_ASSERT(!isValid(client_id)) return;
    add_badge_StoC msg;
    if (!prep_add_badge(&msg, badge_id, agent_id)) return;
    msg.sendToClient(client_id);
}

inline void add_badge_StoC::handle() {}
#endif

#if DC_CLIENT
inline void add_badge_StoC::handle()
{
    class Agents::Agent* a = Agents::get_agent((AgentID)this->agent_id);
    if (a == NULL) return;
    
    a->status.add_badge((BadgeID)this->badge_id);
}
#endif


/******************
** CONFIGURATION **
******************/

void register_badges()
{
    using TextureSheetLoader::badge_texture_alias;
    SpriteSheet b2 = badge_texture_alias(MEDIA_PATH "sprites/badges/badges02.png");
    
    add_badge("debug", b2, 3, 1);
    add_badge("developer", b2, 2, 1);
    add_badge("paid_alpha_user", b2, 1, 1);

    #if DC_CLIENT
    TextureSheetLoader::init_badge_texture();
    TextureSheetLoader::save_badge_texture();
    #endif
    
    verify_badge_conf();  
}


}   // Badges
