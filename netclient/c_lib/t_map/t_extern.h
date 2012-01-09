
#include <compat_gl.h>

extern int must_lock_block_surface;
extern SDL_Surface* block_surface;
extern SDL_PixelFormat *block_surface_pixel_format;
extern int block_surface_width, block_surface_height;
extern GLuint block_texture;
extern GLuint block_texture_no_gamma_correction;
