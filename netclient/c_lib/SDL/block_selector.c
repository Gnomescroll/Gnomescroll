
#include "block_selector.h"


typedef struct {
    int tex;
    int w;
    int h;
} Texture;

Texture hud_texture;
void _load_hud_texture(char *file) {
    SDL_Surface *surface;

    surface = _load_image(file);
    int tex = _create_hud_texture2(surface);

    hud_texture.tex = tex;
    hud_texture.w = surface->w;
    hud_texture.h = surface->h;
}

void _draw_loaded_hud_texture(int x, int y) {
    int x1, y1;

    x = x - hud_texture.w/2;
    y = y - hud_texture.h/2;

    x1 = x + hud_texture.w;
    y1 = y + hud_texture.h;

    _blit_sprite2(hud_texture.tex, x, y, x1, y1, 0);
}

Texture block_selector_texture;
void _load_block_selector_texture(char *file, int scale) {
    SDL_Surface *surface;

    surface = _load_image(file);
    int tex = _create_hud_texture2(surface);

    block_selector_texture.tex = tex;
    block_selector_texture.w = surface->w / scale;
    block_selector_texture.h = surface->h / scale;
}

void _draw_block_selector(int x, int y) {
    int x1, y1;
    x1 = block_selector_texture.h + x;
    y1 = block_selector_texture.w + y;
    _blit_sprite2(block_selector_texture.tex, x, y1, x1, y, 0);
}

int _create_block_texture(char *file) {
    return 0;
}
