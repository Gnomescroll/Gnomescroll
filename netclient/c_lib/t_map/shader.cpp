#include "shader.hpp"

#include "settings.hpp"
#include "texture.hpp"

namespace t_map
{ 

    void init_shaders()
    {
        void set_map_shader_0();
        void init_map_3d_texture();
    }

    void set_map_shader_0() 
    {
        const int index = 0;    //shader index
        const int DEBUG = 1;

        map_shader[index] = glCreateProgramObjectARB();
        map_vert_shader[index] = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        map_frag_shader[index] = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

        char *vs, *fs;

        if(DEBUG) printf("set_map_shader_0: \n");

        vs = textFileRead((char*) "./media/shaders/terrain/terrain_map_mipmap_bilinear_ao.vsh");
        fs = textFileRead((char*) "./media/shaders/terrain/terrain_map_mipmap_bilinear_ao.fsh");

        glShaderSourceARB(map_vert_shader[index], 1, (const GLcharARB**)&vs, NULL);
        glShaderSourceARB(map_frag_shader[index], 1, (const GLcharARB**)&fs, NULL);
        glCompileShaderARB(map_vert_shader[index]);
        if(DEBUG) printShaderInfoLog(map_vert_shader[index]);

        glCompileShaderARB(map_frag_shader[index]);
        if(DEBUG) printShaderInfoLog(map_frag_shader[index]);
        
        glAttachObjectARB(map_shader[index], map_vert_shader[index]);
        glAttachObjectARB(map_shader[index], map_frag_shader[index]);

        glLinkProgramARB(map_shader[index]);

        if(DEBUG) printProgramInfoLog(map_shader[index]);
        
        map_TexCoord = glGetAttribLocation(map_shader[index], "InTexCoord");
        map_LightMatrix = glGetAttribLocation(map_shader[index], "InLightMatrix"); 
    }

    void init_map_3d_texture()
    {
        terrain_map_surface=IMG_Load("media/texture/blocks_01.png");
        if(!terrain_map_surface) {printf("IMG_Load: %s \n", IMG_GetError());return;}

        GLuint internalFormat = GL_SRGB8_ALPHA8_EXT; //GL_RGBA;
        
        GLuint format;
        if (terrain_map_surface->format->Rmask == 0x000000ff) format = GL_RGBA;
        if (terrain_map_surface->format->Rmask != 0x000000ff) format = GL_BGRA;

        int w = 32;
        int h = 32;
        int d = 256;
        Uint32* Pixels = new Uint32[w * h * d];

        SDL_LockSurface(terrain_map_surface);
        int index; Uint32 pix;
        //I have to load them in anyways...

        for(int _i=0; _i < 16; _i++) {
        for(int _j=0; _j < 16; _j++) {
            index = _i + 16*_j;

            for(int i=0; i < 32; i++) {
            for(int j=0; j < 32; j++) {
                pix = ((Uint32*) terrain_map_surface->pixels)[ 512*(j+32*_j) + (i+32*_i) ];
                Pixels[ 32*32*index + (j*32+i) ] = pix;
            }
            }
        }
        }
        SDL_UnlockSurface(terrain_map_surface);
                   
        glEnable(GL_TEXTURE_2D);

        glGenTextures( 1, &terrain_map_glsl );
        glBindTexture(GL_TEXTURE_2D_ARRAY, terrain_map_texture);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if( T_MAP_3D_TEXTURE_MIPMAPS == 0)
        {
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, T_MAP_MAG_FILTER ? GL_NEAREST : GL_LINEAR);
        } 
        else 
        {
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, T_MAP_MAG_FILTER ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR );
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 6);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_GENERATE_MIPMAP, GL_TRUE);
        }
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, w, h, d, 0, format, GL_UNSIGNED_BYTE, Pixels);
        glDisable(GL_TEXTURE_2D);
    }
    
}