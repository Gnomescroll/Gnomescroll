#include "shader.hpp"

#include <t_map/glsl/settings.hpp>
#include <t_map/glsl/texture.hpp>

#include <SDL/texture_sheet_loader.hpp>

/*
In GL 3.0, GL_GENERATE_MIPMAP is deprecated, and in 3.1 and above, it was removed. 
So for those versions, you must use glGenerateMipmap.

GL_GENERATE_MIPMAP is supported for 1.4 and over
*/

//glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); 

namespace t_map
{ 


void MapShader::init_texture()
{

    if (terrain_map_glsl != 0)
    {
        printf("map_shader init_texture: attempting to delete, may cause segfault \n");
        glDeleteTextures(1,&terrain_map_glsl);
        terrain_map_glsl = 0;
    }


    //printf("init_map_3d_texture: 1 \n");
    //glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &terrain_map_glsl);
    glBindTexture(GL_TEXTURE_2D_ARRAY, terrain_map_glsl);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //GL_MIRRORED_REPEAT
    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP);
    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP);

    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY_LARGEST_SUPPORTED);

    if (ANISOTROPIC_FILTERING)
    {
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY_LARGEST_SUPPORTED);
    }

    if (T_MAP_TEXTURE_2D_ARRAY_MIPMAPS == 0)
    {
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, T_MAP_MAG_FILTER ? GL_NEAREST : GL_LINEAR);

        switch(T_MAP_MAG_FILTER)
        {
            case 0:
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST); break;
            case 1:
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;
            default:
                printf("Error: T_MAP_MAG_FILTER value %i invalid for non-mipmapped GL_TEXTURE_2D_ARRAY \n", T_MAP_MAG_FILTER);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;
        }

    } 
    else 
    {
        //GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, and GL_NEAREST_MIPMAP_NEAREST
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        
        //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);


        //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, T_MAP_MAG_FILTER ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR);
        
        //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

        switch(T_MAP_MAG_FILTER)
        {
            case 0:
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); break;
            case 1:
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); break;
            case 2:
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); break;
            case 3:
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); break;
            default:
                printf("Error: T_MAP_MAG_FILTER value %i invalid for mipmapped GL_TEXTURE_2D_ARRAY \n", T_MAP_MAG_FILTER);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;
        }

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 8);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_GENERATE_MIPMAP, GL_TRUE);
    }

    GS_ASSERT(TextureSheetLoader::CubeSurface != NULL);
    if (TextureSheetLoader::CubeSurface == NULL) return;
    
    GLenum format = GL_BGRA;
    if (TextureSheetLoader::CubeSurface->format->Rmask == 0x000000ff)
        format = GL_RGBA;

    GLuint internalFormat = GL_SRGB8_ALPHA8_EXT; //GL_RGBA;
    //GLuint internalFormat = GL_SRGB8_ALPHA8; //GL_RGBA;

    const int w = 32;
    const int h = 32;
    const int d = 256;

    GS_ASSERT(TextureSheetLoader::CubeTextureStack != NULL);
    if (TextureSheetLoader::CubeTextureStack == NULL) return;
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, w, h, d, 0, format, GL_UNSIGNED_BYTE, TextureSheetLoader::CubeTextureStack);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    glDisable(GL_TEXTURE_2D);
}

void MapCompatibilityShader::init_texture()
{


}


SDL_Surface *terrain_map_surface = NULL;
GLuint terrain_map_texture = 0;

GLuint block_textures_compatibility = 0; //use for intel model

void init_shaders()
{
    //determine support for anisotropic filtering
    //if (true || !GLEW_texture_array)

    if (GLEW_EXT_texture_array)
    {
        if (PRODUCTION) printf("Video cards reports GL_EXT_texture_array as supported \n");
    }
    else
    {
        printf("!!! Warning: GL_EXT_texture_array not supported.  Using Backup Shader! \n");
        T_MAP_BACKUP_SHADER = 1;
    }

    if (!GLEW_EXT_texture_sRGB)
    {
        printf("!!! Warning: EXT_texture_sRGB not supported. \n");
    }

    //ANISOTROPIC_FILTERING = 0;

    if (ANISOTROPIC_FILTERING)
    {
        if (GLEW_EXT_texture_filter_anisotropic) // ANISOTROPY_EXT
        {
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ANISOTROPY_LARGEST_SUPPORTED);
            printf("anisotropic filtering supported: max supported= %f \n", ANISOTROPY_LARGEST_SUPPORTED);
        } 
        else 
        {
            printf("anisotropic filtering not supported ! \n");
            ANISOTROPIC_FILTERING = 0;
        }
    }

    T_MAP_BACKUP_SHADER = 1;

    if (T_MAP_BACKUP_SHADER == 0)
    {
        //set_map_shader_0();
        //init_map_3d_texture();

        map_shader.init_texture();
        map_shader.init_shader();
        //true on error
        //if (shader_error_occured(map_shader.shader->shader) == true)
        
        if( map_shader.shader->shader_valid == false)
        {
            printf("!!! Default map shader failed. Setting backup shader \n");
            T_MAP_BACKUP_SHADER = 1;
        }
    }

    if (T_MAP_BACKUP_SHADER == 1)
    {
        ANISOTROPIC_FILTERING = 0;  //disable anisotropic filtering
        init_map_3d_texture_compatibility();

        printf("!!! Warning: Using Intel GPU Compatability mode shader level 0\n");
        
        map_compatibility_shader.init_texture();
        map_compatibility_shader.init_shader(0);

        if (  map_compatibility_shader.shader->shader_valid == true)
        {
            map_compatibility_shader.init_shader(1);

            printf("!!! shader level 0 failed.  Using backup shader level 1 \n");
        }
    }

    init_block_texture_normal();
}

/*
void set_map_shader_0() 
{
    const int index = 0;    //shader index
    const int DEBUG1 = 1;

    map_shader[index] = glCreateProgramObjectARB();
    map_vert_shader[index] = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    map_frag_shader[index] = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    char *vs, *fs;

    if (DEBUG1) printf("set_map_shader_0: \n");

    vs = textFileRead("./media/shaders/terrain/terrain_map_mipmap_bilinear_ao.vsh");
    fs = textFileRead("./media/shaders/terrain/terrain_map_mipmap_bilinear_ao.fsh");

    glShaderSourceARB(map_vert_shader[index], 1, (const GLcharARB**)&vs, NULL);
    glShaderSourceARB(map_frag_shader[index], 1, (const GLcharARB**)&fs, NULL);
    glCompileShaderARB(map_vert_shader[index]);

    if (DEBUG1) printShaderInfoLog(map_vert_shader[index]);

    glCompileShaderARB(map_frag_shader[index]);
    if (DEBUG1) printShaderInfoLog(map_frag_shader[index]);
    
    glAttachObjectARB(map_shader[index], map_vert_shader[index]);
    glAttachObjectARB(map_shader[index], map_frag_shader[index]);

    glLinkProgramARB(map_shader[index]);

    if (DEBUG1) printProgramInfoLog(map_shader[index]);
    
    //uniforms

    map_ChunkPosition = glGetUniformLocation(map_shader[index], "ChunkPosition");
    //map_NormalArray = glGetUniformLocation(map_shader[index], "NormalArray");

    //attributes
    map_Vertex = glGetAttribLocation(map_shader[index], "InVertex");
    map_TexCoord = glGetAttribLocation(map_shader[index], "InTexCoord");
    map_RGB = glGetAttribLocation(map_shader[index], "InRGB");
    //map_Normal = glGetAttribLocation(map_shader[index], "InNormal");

    map_LightMatrix = glGetAttribLocation(map_shader[index], "InLightMatrix"); 
    map_Light = glGetAttribLocation(map_shader[index], "InLight"); 

    //printf("s1= %i s2= %i \n", map_TexCoord, map_LightMatrix);

    free(vs);
    free(fs);
}
*/

void toggle_3d_texture_settings()
{
    static int s = 0;
    s = (s+1) % 6;

    switch (s)
    {
        case 0:
            T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = 0;
            T_MAP_MAG_FILTER  = 0;
            break;
        case 1:
            T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = 0;
            T_MAP_MAG_FILTER  = 1;
            break;
        case 2:
            T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = 1;
            T_MAP_MAG_FILTER  = 0;
            break;
        case 3:
            T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = 1;
            T_MAP_MAG_FILTER  = 1;
            break;
        case 4:
            T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = 1;
            T_MAP_MAG_FILTER  = 2;
            break;
        case 5:
            T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = 1;
            T_MAP_MAG_FILTER  = 3;
            break;
        default:
            printf("toggle_3d_texture_settings: error \n");
            T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = 1;
            T_MAP_MAG_FILTER  = 0;
    }
    printf("TEXTURE_SETTING: T_MAP_TEXTURE_2D_ARRAY_MIPMAPS = %i T_MAP_MAG_FILTER = %i \n", T_MAP_TEXTURE_2D_ARRAY_MIPMAPS,T_MAP_MAG_FILTER);
    init_map_3d_texture();
}

//warning: random segfault on start in graphics driver
void init_map_3d_texture()
{
    //printf("init_map_3d_texture: 0 \n");

}

void teardown_shader()
{
    if (terrain_map_surface != NULL)
        SDL_FreeSurface(terrain_map_surface);
}

void set_map_shader_0_compatibility(int level) 
{
#if 0
    const int index = 0;    //shader index
    const int DEBUG1 = 1;

    map_shader[index] = glCreateProgramObjectARB();
    map_vert_shader[index] = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    map_frag_shader[index] = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    char *vs, *fs;

    if (DEBUG1) printf("set_map_shader_0_compatibility: level %i \n", level);

    bool mesa = false;
    const char* gl_v = (const char*)glGetString(GL_VERSION);
    if (strstr(gl_v, "Mesa") != NULL)
    {
        printf("%s: using Mesa driver shader\n", __FUNCTION__);
        mesa = true;
    }


    if (level == 0)
    {
        if (mesa)
        {
            vs = textFileRead("./media/shaders/terrain/terrain_map_mesa.vsh");
            fs = textFileRead("./media/shaders/terrain/terrain_map_mesa.fsh");
        }
        else
        {
            vs = textFileRead("./media/shaders/terrain/terrain_map_bilinear_ao_compatibility.vsh");
            fs = textFileRead("./media/shaders/terrain/terrain_map_bilinear_ao_compatibility.fsh");
        }
    }
    else
    {
        vs = textFileRead("./media/shaders/terrain/terrain_map_bilinear_ao_compatibility_backup.vsh");
        fs = textFileRead("./media/shaders/terrain/terrain_map_bilinear_ao_compatibility_backup.fsh"); 
    }

    glShaderSourceARB(map_vert_shader[index], 1, (const GLcharARB**)&vs, NULL);
    glShaderSourceARB(map_frag_shader[index], 1, (const GLcharARB**)&fs, NULL);
    glCompileShaderARB(map_vert_shader[index]);
    if (DEBUG1) printShaderInfoLog(map_vert_shader[index]);

    glCompileShaderARB(map_frag_shader[index]);
    if (DEBUG1) printShaderInfoLog(map_frag_shader[index]);
    
    glAttachObjectARB(map_shader[index], map_vert_shader[index]);
    glAttachObjectARB(map_shader[index], map_frag_shader[index]);

    glLinkProgramARB(map_shader[index]);

    if (DEBUG1) printProgramInfoLog(map_shader[index]);
    
    //uniforms

    map_ChunkPosition = glGetUniformLocation(map_shader[index], "ChunkPosition");
    map_NormalArray = glGetUniformLocation(map_shader[index], "NormalArray");

    //attributes
    map_Vertex = glGetAttribLocation(map_shader[index], "InVertex");
    map_TexCoord = glGetAttribLocation(map_shader[index], "InTexCoord");
    map_RGB = glGetAttribLocation(map_shader[index], "InRGB");
    map_Normal = glGetAttribLocation(map_shader[index], "InNormal");

    map_LightMatrix = glGetAttribLocation(map_shader[index], "InLightMatrix"); 
    map_Light = glGetAttribLocation(map_shader[index], "InLight");

    free(vs);
    free(fs);

    CHECK_GL_ERROR();
#endif
}

void init_map_3d_texture_compatibility()
{
#if 0
    SDL_Surface* s = TextureSheetLoader::CubeSurface;

    if (s == NULL)
    {
        printf("init_map_3d_texture_compatibility() error \n");
        return;
    }


    glEnable(GL_TEXTURE_2D);

    if (block_textures_compatibility == 0)
    {
        glGenTextures(1, &block_textures_compatibility);
    }

    glBindTexture(GL_TEXTURE_2D, block_textures_compatibility);

    // Set the texture's stretching properties

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    GLuint internalFormat = GL_SRGB8_ALPHA8_EXT; //GL_RGBA;
    
    //if (s->format->Rmask == 0x000000ff) format = GL_RGBA;
    //if (s->format->Rmask != 0x000000ff) format = GL_BGRA;

    // Edit the texture object's image data using the information SDL_Surface gives us
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, terrain_map_surface->w, terrain_map_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, terrain_map_surface->pixels); //2nd parameter is level
    
    if (ANISOTROPIC_FILTERING)
    {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY_LARGEST_SUPPORTED);
    }

    GLenum texture_format;
    if (s->format->Rmask == 0x000000ff)
        texture_format = GL_RGBA;
    else
        texture_format = GL_BGRA;

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, texture_format, GL_UNSIGNED_BYTE, s->pixels); //2nd parameter is level

    glDisable(GL_TEXTURE_2D);
#endif
}

void init_block_texture_normal()
{

    SDL_Surface* s = TextureSheetLoader::CubeSurface;

    if (s == NULL)
    {
        printf("init_block_texture_normal() error \n");
    }

    glEnable(GL_TEXTURE_2D);

    if (block_textures_normal == 0)
    {
        glGenTextures(1, &block_textures_normal);
    }

    glBindTexture(GL_TEXTURE_2D, block_textures_normal);

    // Set the texture's stretching properties

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GLenum texture_format;
    if (s->format->Rmask == 0x000000ff)
        texture_format = GL_RGBA;
    else
        texture_format = GL_BGRA;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, texture_format, GL_UNSIGNED_BYTE, s->pixels); //2nd parameter is level
    
    glDisable(GL_TEXTURE_2D);

}

}   // t_map
