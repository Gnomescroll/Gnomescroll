#include "hud.hpp"

#include <compat_gl.h>
#include <c_lib/SDL/draw_functions.h>

#include <c_lib/hud/reticle.hpp>

void draw_hud_textures(bool zoom)
{
    if (zoom)
        HudReticle::scope_reticle.draw();
    else
        HudReticle::reticle.draw();
}

void draw_hud_text(bool zoom)
{}
