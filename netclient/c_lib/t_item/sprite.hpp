#pragma once

#include <compat_gl.h>

namespace t_item
{

extern SDL_Surface* ItemSheetSurface;
extern GLuint ItemSheetTexture;

void draw_init();
void draw_teardown();


}  