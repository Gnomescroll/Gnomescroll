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

    SDL_Surface *terrain_map_surface = NULL;
    GLuint terrain_map_texture = 0;

    GLuint block_textures_compatibility = 0; //use for intel model

    void init_shaders()
    {
        //determine support for anisotropic filtering
        //if( true || !GLEW_texture_array)
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

        //T_MAP_BACKUP_SHADER = 1;

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
        init_block_texture_normal();
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

    //warning: random segfault on start in graphics driver
    void init_map_3d_texture()
    {
        printf("init_map_3d_texture: 0 \n");
        /*
            Cleanup
        */
        if(terrain_map_surface != NULL) SDL_FreeSurface(terrain_map_surface);
        
        if(terrain_map_glsl != 0)
        {
            printf("init_map_3d_texture: attempting to delete, may cause segfault \n");
            glDeleteTextures(1,&terrain_map_glsl);
            terrain_map_glsl = 0;
        }


        printf("init_map_3d_texture: 1 \n");
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

        printf("init_map_3d_texture: 2 \n");

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

        printf("init_map_3d_texture: 3 \n");

        // crash happens here

        GLuint format = GL_RGBA;
        GLuint internalFormat = GL_SRGB8_ALPHA8_EXT; //GL_RGBA;
        //GLuint internalFormat = GL_SRGB8_ALPHA8; //GL_RGBA;

        const int w = 32;
        const int h = 32;
        const int d = 256;

        if(TextureSheetLoader::CubeTextureStack == NULL) printf("!!! ERRROR !!! \n");
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, w, h, d, 0, format, GL_UNSIGNED_BYTE, TextureSheetLoader::CubeTextureStack);

        glDisable(GL_TEXTURE_2D);


        printf("init_map_3d_texture: 4 \n");
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

        SDL_Surface* s = TextureSheetLoader::CubeTexture;

        if(s == NULL)
        {
            printf("init_map_3d_texture_comptability() error \n");
        }


        glEnable(GL_TEXTURE_2D);

        if(block_textures_compatibility == 0)
        {
            glGenTextures( 1, &block_textures_compatibility );
        }

        glBindTexture( GL_TEXTURE_2D, block_textures_compatibility );

        // Set the texture's stretching properties

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

        GLuint internalFormat = GL_SRGB8_ALPHA8_EXT; //GL_RGBA;
        GLuint format;
        
        //if (s->format->Rmask == 0x000000ff) format = GL_RGBA;
        //if (s->format->Rmask != 0x000000ff) format = GL_BGRA;

        format = GL_RGBA;
        // Edit the texture object's image data using the information SDL_Surface gives us
        //glTexImage2D(GL_TEXTURE_2D, 0, 4, terrain_map_surface->w, terrain_map_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, terrain_map_surface->pixels ); //2nd parameter is level
        
        if(ANISOTROPIC_FILTERING)
        {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY_LARGEST_SUPPORTED);
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels ); //2nd parameter is level
        
        glDisable(GL_TEXTURE_2D);

    }

    void init_block_texture_normal()
    {

        SDL_Surface* s = TextureSheetLoader::CubeTexture;

        if(s == NULL)
        {
            printf("init_block_texture_normal() error \n");
        }

        glEnable(GL_TEXTURE_2D);

        if(block_textures_normal == 0)
        {
            glGenTextures( 1, &block_textures_normal );
        }

        glBindTexture( GL_TEXTURE_2D, block_textures_normal );

        // Set the texture's stretching properties

        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

        GLuint internalFormat = GL_RGBA; //GL_RGBA;
        GLuint format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels ); //2nd parameter is level
        
        glDisable(GL_TEXTURE_2D);

    }


    void init_block_item_sheet()
    {
        unsigned int color_tex, fb, depth_rb;
        int xres = 512;
        int yres = 512;

        //RGBA8 2D texture, 24 bit depth texture, 256x256
        glGenTextures(1, &color_tex);
        glBindTexture(GL_TEXTURE_2D, color_tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //NULL means reserve texture memory, but texels are undefined
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, xres,yres, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
        //-------------------------
        glGenFramebuffersEXT(1, &fb);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
        //Attach 2D texture to this FBO
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex, 0);
        //-------------------------
        glGenRenderbuffersEXT(1, &depth_rb);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, xres,yres);
        //-------------------------
        //Attach depth buffer to FBO
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb);
        //-------------------------
        //Does the GPU support current FBO configuration?
        GLenum status;
        status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        switch(status)
        {
            case GL_FRAMEBUFFER_COMPLETE_EXT:
            printf("FBO works\n");
            break;

            default:
            printf("FBO error\n");
        }
        //-------------------------
        //and now you can render to GL_TEXTURE_2D
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //-------------------------
        glViewport(0, 0, xres, yres);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, (float) xres, 0.0, (float) yres, -1.0, 1.0); 
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        //-------------------------
        //glDisable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);

        glBindTexture( GL_TEXTURE_2D, t_map::block_textures_normal);

        glBegin(GL_QUADS);
        draw_iso_cube(32.0,32.0, 48,48,48);
        glEnd();

        char* PBUFFER = (char*) malloc(4*xres*yres);
        glReadPixels(0, 0, xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, (void*) PBUFFER);

        //-------------------------
        //**************************
        //RenderATriangle, {0.0, 0.0}, {256.0, 0.0}, {256.0, 256.0}
        //Read http://www.opengl.org/wiki/VBO_-_just_examples
        //RenderATriangle();
        //-------------------------
        //GLubyte pixels[4*4*4];
        //glReadPixels(0, 0, 4, 4, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
        //pixels 0, 1, 2 should be white
        //pixel 4 should be black
        //----------------
        //Bind 0, which means render to back buffer
        glBindFramebufferEXT(GL_FRAMEBUFFER, 0);



        glBindTexture( GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); 
        glViewport (0, 0, _xres, _yres);


        char FileName[128];

        sprintf(FileName,"./screenshot/%s.png",  (char*) "fbo_test");

        for(int i=0; i < xres; i++)
        for(int j=0; j < yres; j++)
        {
            PBUFFER[4*(xres*j + i) + 3] = 255;
        }

        {
            int index;
            void* temp_row;
            int height_div_2;

            temp_row = (void *)malloc(4*xres);
            if(NULL == temp_row)
            {
                SDL_SetError("save_screenshot: not enough memory for surface inversion");
            }
            int pitch = xres * 4;
            //int w = xres;
            int h = yres;

            height_div_2 = (int) (yres * .5);
            for(index = 0; index < height_div_2; index++)    
            {
                memcpy( (Uint8 *)temp_row, (Uint8 *)(PBUFFER) + pitch * index, pitch);
                memcpy( (Uint8 *)(PBUFFER) + pitch * index, (Uint8 *)PBUFFER + pitch * (h - index-1), pitch);
                memcpy( (Uint8 *)(PBUFFER) + pitch * (h - index-1), temp_row, pitch);
            }
            free(temp_row); 
        }

        size_t png_size;
        char* PNG_IMAGE = (char* ) tdefl_write_image_to_png_file_in_memory(
            (const char*) PBUFFER, xres, yres, 4, &png_size);
        FILE * pFile;
        pFile = fopen ( FileName , "wb" );
        fwrite (PNG_IMAGE , 1 , png_size, pFile );
        fclose (pFile);

        free(PNG_IMAGE);
        free(PBUFFER);

#if 0

        glColor4ub(255, 255, 255, 255);
        glColor3ub(255, 0,0);

        //glBindTexture( GL_TEXTURE_2D, t_map::block_textures_normal);

        glDisable(GL_TEXTURE_2D);

        glBegin(GL_QUADS);
        draw_iso_cube(35.0,35.0, 0,0,0);
        glEnd();

    /*
        Start Shader
    */

    int xres = 512;
    int yres = 512;

    char FileName[128];

    sprintf(FileName,"./screenshot/%s.png",  (char*) "fbo_test");

    //printf("Screenshot: %s \n", FileName);

    //SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, xres, yres,
    //                                               32, 0x0000ff, 0x00ff00, 0xff0000, 0x000000);

    char* PBUFFER = (char*) malloc(4*xres*yres);

    glReadPixels(0, 0, _xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, (void*) PBUFFER);

    for(int i=0; i < xres; i++)
    for(int j=0; j < yres; j++)
    {
        PBUFFER[4*(xres*j + i) + 3] = 255;
    }

    {
        int index;
        void* temp_row;
        int height_div_2;

        temp_row = (void *)malloc(4*xres);
        if(NULL == temp_row)
        {
            SDL_SetError("save_screenshot: not enough memory for surface inversion");
        }
        int pitch = xres * 4;
        //int w = xres;
        int h = yres;

        height_div_2 = (int) (yres * .5);
        for(index = 0; index < height_div_2; index++)    
        {
            memcpy( (Uint8 *)temp_row, (Uint8 *)(PBUFFER) + pitch * index, pitch);
            memcpy( (Uint8 *)(PBUFFER) + pitch * index, (Uint8 *)PBUFFER + pitch * (h - index-1), pitch);
            memcpy( (Uint8 *)(PBUFFER) + pitch * (h - index-1), temp_row, pitch);
        }
        free(temp_row); 
    }

    size_t png_size;
    char* PNG_IMAGE = (char* ) tdefl_write_image_to_png_file_in_memory(
        (const char*) PBUFFER, xres, yres, 4, &png_size);
    FILE * pFile;
    pFile = fopen ( FileName , "wb" );
    fwrite (PNG_IMAGE , 1 , png_size, pFile );
    fclose (pFile);

    free(PNG_IMAGE);
    free(PBUFFER);

    //#endif
    glViewport (0, 0, _xres, _yres);

    //glBindTexture( GL_TEXTURE_2D, 0);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0); //set render target back

/*
        int error = glGetError();
        printf("Error Status: %i \n", error);
        printf("error: %s \n", gluErrorString(error) );
        abort();
*/
#endif
    }
}
