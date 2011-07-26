
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
//extern int x;'

int _init_image_loader();
SDL_Surface* _load_image(char *file);
int _create_hud_texture(char *file);
int _create_block_texture(char *file);
int _create_texture(SDL_Surface* surface);
