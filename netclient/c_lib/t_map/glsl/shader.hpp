#pragma once


namespace t_map
{ 
    void init_shaders();
    
    void set_map_shader_0();
    void init_map_3d_texture();

    const int num_map_shaders = 1;

    GLenum map_vert_shader[num_map_shaders] = {0};
    GLenum map_frag_shader[num_map_shaders] = {0};
    GLenum map_shader[num_map_shaders] = {0};

    //shader 0
    int map_TexCoord;
    int map_LightMatrix;

    //texture
    extern SDL_Surface *terrain_map_surface;
    extern GLuint terrain_map_texture;

    void teardown_shader(); // frees surface
}
