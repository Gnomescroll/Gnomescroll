#pragma once

#if DC_CLIENT
#include <SDL/texture_sheet_loader.hpp>
#include <t_map/glsl/palette.hpp>
#include <t_map/glsl/texture.hpp>
#include <SDL/draw_functions.hpp>
#endif

namespace t_map
{

void blit_block_item_sheet()
{
    GLuint block_item_64_texture = 0;
    {
        unsigned int color_tex, fb, depth_rb;
        const int xres = 1024;
        const int yres = 1024;
        const float scale = 64.0f;

        //RGBA8 2D texture, 24 bit depth texture, 256x256
        glGenTextures(1, &color_tex);
        glBindTexture(GL_TEXTURE_2D, color_tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //NULL means reserve texture memory, but texels are undefined
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xres,yres, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
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
            //printf("FBO works\n");
            break;

            default:
            printf("blit_block_item_sheet: FBO error\n");
            break;
        }
        //-------------------------
        //and now you can render to GL_TEXTURE_2D
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //-------------------------
        glViewport(0, 0, xres, yres);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0f, (float) xres, 0.0f, (float) yres, -1.0f, 1.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        //-------------------------
        //glDisable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_2D);
        GL_ASSERT(GL_BLEND, false);
        glDisable(GL_DEPTH_TEST);

        glBindTexture(GL_TEXTURE_2D, t_map::block_textures_normal);

        glBegin(GL_QUADS);
        for (int i=0; i<16; i++)
        for (int j=0; j<16; j++)
        {
            /*
                const int T = 0;
                const int B = 1;
                const int N = 2;
                const int S = 3;
                const int W = 4;
                const int E = 5;
            */

            int index = 16*j+i;
            // the NULL_CUBE is reserved at 255 (so that it fits in a packet)
            // must skip it
            IF_INVALID_CUBE_TYPE((CubeType)index) continue;

            int s1 = get_cube_side_texture((CubeType)index, 0); //T
            int s2 = get_cube_side_texture((CubeType)index, 2); //N
            int s3 = get_cube_side_texture((CubeType)index, 4); //W

            draw_iso_cube(i*scale, j*scale, scale, s1,s2,s3);
        }
        glEnd();

        block_item_64_surface = create_surface_from_nothing(xres, yres);

        SDL_LockSurface(block_item_64_surface);

        GLenum format = GL_BGRA;
        if (block_item_64_surface->format->Rmask == 0X000000FF)
            format = GL_RGBA;

        //glReadPixels(0, 0, xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, (void*) block_item_64_surface->pixels);
        glReadPixels(0, 0, xres, yres, format, GL_UNSIGNED_BYTE, (void*) block_item_64_surface->pixels);

        SDL_UnlockSurface(block_item_64_surface);

        save_surface_to_png(block_item_64_surface, "screenshot/fbo_test_64.png");

        //Delete resources
        glDeleteTextures(1, &color_tex);
        glDeleteRenderbuffersEXT(1, &depth_rb);
        //Bind 0, which means render to back buffer, as a result, fb is unbound
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glDeleteFramebuffersEXT(1, &fb);

        //http://stackoverflow.com/questions/5102277/resizing-an-image-using-opengl
        // setup another framebuffer here
        // bind the 64 pixel texture
        // draw to a 256x256 framebuffer
        // save surface to png again

        create_texture_from_surface(block_item_64_surface, &block_item_64_texture, GL_NEAREST);
        //create_texture_from_surface(block_item_64_surface, &block_item_64_texture, GL_LINEAR);
        GS_ASSERT_ABORT(block_item_64_texture != 0);
    }

    {
        unsigned int color_tex, fb, depth_rb;
        const int xres = 256;
        const int yres = 256;
        //const float scale = 16.0;

        //RGBA8 2D texture, 24 bit depth texture, 256x256
        glGenTextures(1, &color_tex);
        glBindTexture(GL_TEXTURE_2D, color_tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //NULL means reserve texture memory, but texels are undefined
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xres,yres, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
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
            //printf("FBO works\n");
            break;

            default:
            printf("blit_block_item_sheet: FBO error\n");
            break;
        }
        //-------------------------
        //and now you can render to GL_TEXTURE_2D
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //-------------------------
        glViewport(0, 0, xres, yres);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0f, (float) xres, 0.0f, (float) yres, -1.0f, 1.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        //-------------------------
        //glDisable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_2D);
        GL_ASSERT(GL_BLEND, false);
        glDisable(GL_DEPTH_TEST);

        //glBindTexture(GL_TEXTURE_2D, t_map::block_textures_normal);

        //glBegin(GL_QUADS);
        //for (int i=0; i<16; i++)
        //for (int j=0; j<16; j++)
        //{
            ///*
                //const int T = 0;
                //const int B = 1;
                //const int N = 2;
                //const int S = 3;
                //const int W = 4;
                //const int E = 5;
            //*/

            //int index = 16*j+i;
            //int s1 = get_cube_side_texture(index, 0); //T
            //int s2 = get_cube_side_texture(index, 2); //N
            //int s3 = get_cube_side_texture(index, 4); //W

            //draw_iso_cube(i*scale, j*scale, scale, s1,s2,s3);
        //}
        //glEnd();

        //glBegin(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, block_item_64_texture);
        draw_bound_texture(0.0f, 0.0f, xres, yres);
        //glDisable(GL_TEXTURE_2D);

        block_item_16_surface = create_surface_from_nothing(xres, yres);

        SDL_LockSurface(block_item_16_surface);

        GLenum format = GL_BGRA;
        if (block_item_16_surface->format->Rmask == 0X000000FF)
            format = GL_RGBA;

        //glReadPixels(0, 0, xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, (void*) block_item_16_surface->pixels);
        glReadPixels(0, 0, xres, yres, format, GL_UNSIGNED_BYTE, (void*) block_item_16_surface->pixels);

        SDL_UnlockSurface(block_item_16_surface);

        save_surface_to_png(block_item_16_surface, "screenshot/fbo_test_16.png");

        //Delete resources
        glDeleteTextures(1, &color_tex);
        glDeleteRenderbuffersEXT(1, &depth_rb);
        //Bind 0, which means render to back buffer, as a result, fb is unbound
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glDeleteFramebuffersEXT(1, &fb);
    }

    glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glViewport (0, 0, _xres, _yres);
}

}   // t_map
