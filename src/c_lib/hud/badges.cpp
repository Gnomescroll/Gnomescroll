#include "badges.hpp"

#include <SDL/texture_sheet_loader.hpp>

namespace Badges
{

size_t n_badges = 0;
class Badge badges[MAX_BADGES];

static void add_badge(const char* filename, SpriteSheet sheet_id, int xpos, int ypos)
{
    IF_ASSERT(n_badges >= MAX_BADGES) return;
    
    GS_ASSERT(filename[0] != '\0');
    GS_ASSERT(strlen(filename) <= BADGE_NAME_MAX_LENGTH);
    GS_ASSERT(sheet_id != NULL_SPRITE_SHEET);
    GS_ASSERT(xpos > 0 && ypos > 0);

    strncpy(badges[n_badges].name, filename, BADGE_NAME_MAX_LENGTH+1);
    badges[n_badges].name[BADGE_NAME_MAX_LENGTH] = '\0';

    int sprite_id = TextureSheetLoader::badge_texture_sheet_loader->blit(sheet_id, xpos, ypos);
    GS_ASSERT(sprite_id != NULL_SPRITE);

    badges[n_badges].sprite_id = sprite_id;

    n_badges++;
}

static void verify_badge_conf()
{
    if (!n_badges) return;
    
    // check valid values
    for (size_t i=0; i<n_badges; i++)
    {
        GS_ASSERT(badges[i].name[0] != '\0');
        GS_ASSERT(badges[i].sprite_id != NULL_SPRITE);
    }

    // check configs against each other for duplicates
    for (size_t i=0; i<n_badges-1; i++)
    for (size_t j=i+1; j<n_badges; j++)
    {
        GS_ASSERT(strcmp(badges[i].name, badges[j].name) != 0);
        GS_ASSERT(badges[i].sprite_id != badges[j].sprite_id);
    }
}

void register_badges()
{
    using TextureSheetLoader::badge_texture_alias;
    SpriteSheet b1 = badge_texture_alias(MEDIA_PATH "sprites/badges/badges1.png");
    
    add_badge("paid_alpha_user", b1, 3, 2);

    TextureSheetLoader::save_badge_texture();
    verify_badge_conf();    
}

}   // Badges
