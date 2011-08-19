'''
cdef extern from "texture_loader.h":
    int _init_image_loader()
    SDL_Surface* _load_image(char *file)
    int _create_hud_texture(char *file) #deprecate
    int _create_block_texture(char *file) #deprecate
    int _create_texture(SDL_Surface* surface)
'''

#cdef extern from "ray_trace/ray_trace.h":
#    int _init_ray_trace()
