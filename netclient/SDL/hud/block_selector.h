#ifndef block_selector_h
#define block_selector_h

#include <compat_gl.h>

#include "math.h"

#include "../texture_loader.h"

int _init_block_selector();

void _draw_block_selector(int x, int y);
void _load_block_selector_texture(char *file, int scale);
int _blit_block_selector(int texture, float x0, float y0, float x1, float y1, float z);

#endif
