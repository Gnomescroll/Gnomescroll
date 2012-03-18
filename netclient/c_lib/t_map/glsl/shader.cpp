#include "shader.hpp"

#include <c_lib/t_map/glsl/settings.hpp>
#include <c_lib/t_map/glsl/texture.hpp>

#include <c_lib/SDL/texture_sheet_loader.hpp>

/*
In GL 3.0, GL_GENERATE_MIPMAP is deprecated, and in 3.1 and above, it was removed. 
So for those versions, you must use glGenerateMipmap.

GL_GENERATE_MIPMAP is supported for 1.4 and over
*/

//glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); 

namespace t_map
{ 

    SDL_Surface *terrain_map_surface = NULL;
    GLuint terrain_map_texture = 0;

    void init_shaders()
    {

        //determine support for anisotropic filtering


        //if( true || !GLEW_EXT_texture_array)
        if(!GLEW_EXT_texture_array)
        {
            printf("!!! Warning: GL_EXT_texture_array not supported.  Using Backup Shader! \n");
            T_MAP_BACKUP_SHADER = 1;

        }

        if(GLEW_EXT_texture_filter_anisotropic && ANISOTROPIC_FILTERING == 1) // ANISOTROPY_EXT
        {
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ANISOTROPY_LARGEST_SUPPORTED);
            printf("anisotropic filtering supported: max supported= %f \n", ANISOTROPY_LARGEST_SUPPORTED);
        } 
        else 
        {
            printf("anisotropic filtering not supported ! \n");
            ANISOTROPIC_FILTERING = 0;
        }

        T_MAP_BACKUP_SHADER = 1;

        // || true
        if(T_MAP_BACKUP_SHADER == 1 )
        {
            printf("!!! Warning: Using Intel GPU Compatability mode shader\n");
            set_map_shader_0_comptability();
            init_map_3d_texture_comptability();
        }
        else
        {
            set_map_shader_0();
            init_map_3d_texture();
        }

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
        
        //uniforms

        map_ChunkPosition = glGetUniformLocation(map_shader[index], "ChunkPosition");
        map_NormalArray = glGetUniformLocation(map_shader[index], "NormalArray");

        //attributes
        map_Vertex = glGetAttribLocation(map_shader[index], "InVertex");
        map_TexCoord = glGetAttribLocation(map_shader[index], "InTexCoord");
        map_RGB = glGetAttribLocation(map_shader[index], "InRGB");
        map_Normal = glGetAttribLocation(map_shader[index], "InNormal");

        map_LightMatrix = glGetAttribLocation(map_shader[index], "InLightMatrix"); 
        
        //printf("s1= %i s2= %i \n", map_TexCoord, map_LightMatrix );

        free(vs);
        free(fs);

    }

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


    void init_map_3d_texture()
    {
        /*
            Cleanup
        */
        if(terrain_map_surface != NULL) 
        SDL_FreeSurface(terrain_map_surface);
        if(terrain_map_glsl != 0)
        glDeleteTextures(1,&terrain_map_glsl);

/*
        terrain_map_surface=IMG_Load("media/texture/blocks_01.png");
        if(!terrain_map_surface) {printf("IMG_Load: %s \n", IMG_GetError());return;}


        
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
*/      

        glEnable(GL_TEXTURE_2D);

        glGenTextures( 1, &terrain_map_glsl );
        glBindTexture(GL_TEXTURE_2D_ARRAY, terrain_map_glsl);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY_LARGEST_SUPPORTED);

        if(ANISOTROPIC_FILTERING)
        {
            glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY_LARGEST_SUPPORTED);
        }

        if( T_MAP_TEXTURE_2D_ARRAY_MIPMAPS == 0)
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
            //glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, T_MAP_MAG_FILTER ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR );
            
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


        GLuint format = GL_RGBA;
        GLuint internalFormat = GL_SRGB8_ALPHA8_EXT; //GL_RGBA;

        const int w = 32;
        const int h = 32;
        const int d = 256;

        if(TextureSheetLoader::CubeTextureStack == NULL) printf("!!! ERRROR !!! \n");
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, w, h, d, 0, format, GL_UNSIGNED_BYTE, TextureSheetLoader::CubeTextureStack);

        

        //glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, w, h, d, 0, format, GL_UNSIGNED_BYTE, Pixels);
        //glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

        glDisable(GL_TEXTURE_2D);

        //delete[] Pixels;
    }

    void teardown_shader()
    {
        if (terrain_map_surface != NULL)
            SDL_FreeSurface(terrain_map_surface);
    }

    void set_map_shader_0_comptability() 
    {
        const int index = 0;    //shader index
        const int DEBUG = 1;

        map_shader[index] = glCreateProgramObjectARB();
        map_vert_shader[index] = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        map_frag_shader[index] = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

        char *vs, *fs;

        if(DEBUG) printf("set_map_shader_0: \n");

        vs = textFileRead((char*) "./media/shaders/terrain/terrain_map_bilinear_ao_comptability.vsh");
        fs = textFileRead((char*) "./media/shaders/terrain/terrain_map_bilinear_ao_comptability.fsh");

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
        
        //uniforms

        map_ChunkPosition = glGetUniformLocation(map_shader[index], "ChunkPosition");
        map_NormalArray = glGetUniformLocation(map_shader[index], "NormalArray");

        //attributes
        map_Vertex = glGetAttribLocation(map_shader[index], "InVertex");
        map_TexCoord = glGetAttribLocation(map_shader[index], "InTexCoord");
        map_RGB = glGetAttribLocation(map_shader[index], "InRGB");
        map_Normal = glGetAttribLocation(map_shader[index], "InNormal");

        map_LightMatrix = glGetAttribLocation(map_shader[index], "InLightMatrix"); 
        
        //printf("s1= %i s2= %i \n", map_TexCoord, map_LightMatrix );

        free(vs);
        free(fs);

    }


    void init_map_3d_texture_comptability()
    {


        glEnable(GL_TEXTURE_2D);

        if(block_textures_compatibility == 0)
        {
            glGenTextures( 1, &block_textures_compatibility );
        }

        glBindTexture( GL_TEXTURE_2D, block_textures_compatibility );

        // Set the texture's stretching properties

        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

        GLuint internalFormat = GL_SRGB8_ALPHA8_EXT; //GL_RGBA;
        GLuint format;
        if (terrain_map_surface->format->Rmask == 0x000000ff) format = GL_RGBA;
        if (terrain_map_surface->format->Rmask != 0x000000ff) format = GL_BGRA;

        // Edit the texture object's image data using the information SDL_Surface gives us
        //glTexImage2D(GL_TEXTURE_2D, 0, 4, terrain_map_surface->w, terrain_map_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, terrain_map_surface->pixels ); //2nd parameter is level
        
        if(ANISOTROPIC_FILTERING)
        {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY_LARGEST_SUPPORTED);
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, terrain_map_surface->w, terrain_map_surface->h, 0, format, GL_UNSIGNED_BYTE, TextureSheetLoader::CubeTexture->pixels ); //2nd parameter is level
        

        glDisable(GL_TEXTURE_2D);

    }


}