#include "hud.hpp"

#include <compat_gl.h>
#include <c_lib/SDL/draw_functions.h>

#include <c_lib/hud/reticle.hpp>
#include <c_lib/hud/cube_selector.hpp>
#include <c_lib/hud/inventory.hpp>

static struct HudDrawSettings
{
    bool zoom;
    bool cube_selector;
    bool inventory;
} hud_draw_settings;

//static HudDrawSettings hud_draw_settings;
void set_hud_draw_settings(bool zoom, bool cube_selector, bool inventory)
{
    printf("A %d %d %d\n", zoom, cube_selector, inventory);
    hud_draw_settings.zoom = zoom;
    hud_draw_settings.cube_selector = cube_selector;
    hud_draw_settings.inventory = inventory;
    printf("B %d %d %d\n", hud_draw_settings.zoom, hud_draw_settings.cube_selector, hud_draw_settings.inventory);
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
