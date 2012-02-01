#include "hud.hpp"

#include <c_lib/hud/reticle.hpp>
#include <c_lib/hud/cube_selector.hpp>
#include <c_lib/hud/inventory.hpp>

static struct HudDrawSettings
{
    bool zoom;
    bool cube_selector;
    bool inventory;
} hud_draw_settings;

void set_hud_draw_settings(bool zoom, bool cube_selector, bool inventory)
{
    hud_draw_settings.zoom = zoom;
    hud_draw_settings.cube_selector = cube_selector;
    hud_draw_settings.inventory = inventory;
}

void draw_hud_textures()
{
    if (hud_draw_settings.zoom)
    {
        HudReticle::scope_reticle.draw();
        return;
    }

    HudReticle::reticle.draw();

    if (hud_draw_settings.cube_selector)
        HudCubeSelector::cube_selector.draw();

    if (hud_draw_settings.inventory)
        HudInventory::inventory.draw();
}

void draw_hud_text()
{}
