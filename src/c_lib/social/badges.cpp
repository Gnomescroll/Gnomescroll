#include "badges.hpp"

#include <SDL/texture_sheet_loader.hpp>
#include <common/common.hpp>
#include <agent/_interface.hpp>
#include <common/dat/properties.hpp>

namespace Badges
{

class Badge: public Property<BadgeType>
{
    public:
        int sprite_id;

    Badge() :
        Property<BadgeType>(NULL_BADGE),
        sprite_id(NULL_SPRITE)
    {}
};

class Badges: public Properties<Badge, BadgeType>
{
    public:

    Badges() :
        Properties<Badge, BadgeType>(MAX_BADGES)
    {}
};


class Badges* badges;

static void add_badge(const char* name, SpriteSheet sheet_id, int xpos, int ypos)
{
    Badge* b = badges->get_next();
    IF_ASSERT(b == NULL) return;

    GS_ASSERT(sheet_id != NULL_SPRITE_SHEET);
    GS_ASSERT(xpos > 0 && ypos > 0);

    b->set_name(name);

    #if DC_CLIENT
    int sprite_id = TextureSheetLoader::badge_texture_sheet_loader->blit(sheet_id, xpos, ypos);
    GS_ASSERT(sprite_id != NULL_SPRITE);
    b->sprite_id = sprite_id;
    #endif
}

static void verify_badge_conf()
{
    for (size_t i=0; i<badges->max; i++)
    {   // check valid values
        Badge* a = &badges->properties[i];
        if (!a->loaded) continue;
        GS_ASSERT(a->name[0] != '\0');
        GS_ASSERT(is_valid_name(a->name));
        #if DC_CLIENT
        GS_ASSERT(a->sprite_id != NULL_SPRITE);
        #endif
    }

    for (size_t i=0; i<badges->max-1; i++)
    for (size_t j=i+1; j<badges->max; j++)
    {   // check configs against each other for duplicates
        Badge* a = &badges->properties[i];
        Badge* b = &badges->properties[j];
        if (!a->loaded || !b->loaded) continue;
        GS_ASSERT(strcmp(a->name, b->name) != 0);
        #if DC_CLIENT
        GS_ASSERT(a->sprite_id != b->sprite_id);
        #endif
    }
}

Badge* get_badge(const char* name)
{
    return badges->get(name);
}

Badge* get_badge(BadgeType type)
{
    return badges->get(type);
}

BadgeType get_badge_type(const char* name)
{
    Badge* b = get_badge(name);
    IF_ASSERT(b == NULL) return NULL_BADGE;
    return b->type;
}

const char* get_badge_name(BadgeType type)
{
    Badge* b = get_badge(type);
    IF_ASSERT(b == NULL) return NULL;
    return b->name;
}

int get_badge_sprite(BadgeType type)
{
    Badge* b = get_badge(type);
    IF_ASSERT(b == NULL) return NULL_SPRITE;
    return b->sprite_id;
}


void init()
{
    GS_ASSERT(badges == NULL);
    badges = new Badges;
}

void teardown()
{
    delete badges;
}

void init_packets()
{
    add_badge_StoC::register_client_packet();
}

#if DC_SERVER
static bool prep_add_badge(add_badge_StoC* msg, BadgeType badge_type, AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return false;
    IF_ASSERT(!isValid(badge_type)) return false;
    msg->badge_type = badge_type;
    msg->agent_id = agent_id;
    return true;
}

void broadcast_badge(BadgeType badge_type, AgentID agent_id)
{
    add_badge_StoC msg;
    if (!prep_add_badge(&msg, badge_type, agent_id)) return;
    msg.broadcast();
}

void send_badge(BadgeType badge_type, AgentID agent_id, ClientID client_id)
{
    IF_ASSERT(!isValid(client_id)) return;
    add_badge_StoC msg;
    if (!prep_add_badge(&msg, badge_type, agent_id)) return;
    msg.sendToClient(client_id);
}

inline void add_badge_StoC::handle() {}
#endif

#if DC_CLIENT
inline void add_badge_StoC::handle()
{
    class Agents::Agent* a = Agents::get_agent((AgentID)this->agent_id);
    if (a == NULL) return;

    a->status.add_badge((BadgeType)this->badge_type);
}
#endif


/******************
** CONFIGURATION **
******************/

void register_badges()
{
    using TextureSheetLoader::badge_texture_alias;
    SpriteSheet b1 = badge_texture_alias(MEDIA_PATH "sprites/badges/badges01.png");

    add_badge("debug", b1, 3, 1);
    add_badge("developer", b1, 2, 1);
    add_badge("paid_alpha_user", b1, 1, 1);

    #if DC_CLIENT
    TextureSheetLoader::init_badge_texture();
    TextureSheetLoader::save_badge_texture();
    #endif

    badges->done_loading();
    verify_badge_conf();
}


}   // Badges
