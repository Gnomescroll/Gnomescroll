#pragma once


#include <t_map/config/_interface.hpp>

namespace t_map 
{

void cube_def(int id, int type, const char* name);
void iso_texture(int tex_id);
void iso_texture(int sheet_id, int ypos, int xpos);
void side_texture(int side, int tex_id);
void side_texture(int side, int sheet_id, int ypos, int xpos);
void hud_def(int hudy, int hudx, int tex_id);
void hud_def(int hudy,int hudx, int sheet_id, int ypos, int xpos);
int texture_alias(const char* spritesheet);



/*
--[[
t top
b bottom
n north
s south
w west
e east
]]
*/

/*
    ErrorBlock,
    EmptyBlock,
    SolidBlock,
    ItemContainerBlock
*/

void load_block_dat()
{
    
    int t0 = texture_alias("media/sprites/t00.png");
    int t1 = texture_alias("media/sprites/t01.png");

    const int T = 0;
    const int B = 1;
    const int N = 2;
    const int S = 3;
    const int W = 4;
    const int E = 5;

    int error_block =  sprite_alias(t0,1,1);

    cube_def(255, ErrorBlock, "error_block");
    //iso_texture(error_block);
    side_texture(T, error_block);
    side_texture(B, error_block);
    side_texture(N, error_block);
    side_texture(S, error_block);
    side_texture(W, error_block);
    side_texture(E, error_block);

    hud_def(1,1, error_block);
/*
    side_texture(T, error_block);
    side_texture(T, t0, 0,0);
*/

    cube_def(0, EmptyBlock, "empty_block");
    iso_texture(error_block);


    cube_def(1, SolidBlock, "terminal_blue");
    iso_texture(t0,1,3);
    hud_def(1,2, t0,1,1);

    cube_def(2, SolidBlock, "terminal_green");
    iso_texture(t0,1,4);
    hud_def(1,3, t0,1,2);


    cube_def(3, SolidBlock, "battery");
    iso_texture(t0,2,3);
    side_texture(T, t0, 2,2);
    side_texture(B, t0, 2,4);
    hud_def(1,4, t0,1,3);


    cube_def(6, ItemContainerBlock, "crate_2");
    iso_texture(t1,2,6);
    side_texture(T, t1, 1,7);
    side_texture(B, t1, 1,7);
    side_texture(B, t1, 1,7);
    hud_def(2,1, t1,1,3);

    cube_def(7, ItemContainerBlock, "crate_3");
    iso_texture(t1,3,4);
    side_texture(T, t1, 2,5);
    side_texture(B, t1, 3,6);
    side_texture(N, t1, 3,5);
    hud_def(2,2,t1,1,3);

    cube_def(16, SolidBlock, "methane_1");
    iso_texture(t1,2,2);
    hud_def(3,1, t1,2,2);

    cube_def(17, SolidBlock, "methane_2");
    iso_texture(t1,3,2);
    hud_def(3,2, t1,3,2);

    cube_def(18, SolidBlock, "methane_3");
    iso_texture(t1,4,3);
    hud_def(3,3, t1,4,3);

    cube_def(19, SolidBlock, "methane_4");
    iso_texture(t1,5,3);
    hud_def(3,4, t1,5,3);

    cube_def(20, SolidBlock, "methane_5");
    iso_texture(t1,4,1);
    hud_def(3,5, t1,4,1);


    cube_def(54, SolidBlock, "regolith");
    iso_texture(t1,10,4);
    side_texture(T, t1,10,3);
    side_texture(B, t1,10,5);
    hud_def(4,1, t1,10,4);

    cube_def(55, SolidBlock, "carbon");
    iso_texture(t1,10,6);
    hud_def(4,2, t1,10,6);

	end_block_dat();
    //end_block_item_sheet();

    blit_block_item_sheet();

}


void blit_block_item_sheet()
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
    glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
    glBindTexture( GL_TEXTURE_2D, 0);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0); 
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

}

}