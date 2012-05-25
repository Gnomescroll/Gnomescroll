#pragma once


#if DC_CLIENT
#include <c_lib/SDL/texture_sheet_loader.hpp>
#endif

/*
    BLOCK DAT
*/

namespace t_map
{
enum CubeType
{
    ErrorBlock,
    EmptyBlock,
    SolidBlock,
    ItemContainerBlock
};

/*
struct cubeProperties 
{
    bool active;
    bool solid;
    bool occludes;
    bool transparent;
    bool item_drop; //does block drop items
    bool item_container;  //inventory and crafting bench blocks
    bool reserved7;
    bool reserved8;

    unsigned char max_damage;
    unsigned char color_type; //flat, discrete and perlin
    //bool solid;
    //bool gravity;
};
*/

/*
enum CubeType
{
    ErrorBlock,
    EmptyBlock,
    SolidBlock,
    ItemContainerBlock
};
*/


int _current_cube_id = 0;
int _current_pallet = 0;

void cube_def(int id, int type, const char* name)
{ 
    _current_cube_id = id;
    set_cube_name(id, (char*) name);


    struct cubeProperties p;
    p.active = true;
    p.solid = true;
    p.occludes = true;
    p.transparent = false;
    p.item_drop = false;
    p.item_container = false;

    p.max_damage = 32;
    p.color_type = 0;

    switch(type)
    {
        case ErrorBlock:
        break;

        case EmptyBlock:
        p.active = false;
        p.solid = false;
        p.occludes = false;
        break;
        case SolidBlock:
        break;

        case ItemContainerBlock:
        p.item_container = true;
        break;

        default:
        GS_ABORT();
    }


    //extern struct cubeProperties* cube_list;
    cube_list[id] = p;
    _current_pallet = 0;

}

void iso_texture(int tex_id)
{
#ifdef DC_CLIENT
    set_cube_side_texture(_current_cube_id, 0, tex_id);
    set_cube_side_texture(_current_cube_id, 1, tex_id);
    set_cube_side_texture(_current_cube_id, 2, tex_id);
    set_cube_side_texture(_current_cube_id, 3, tex_id);
    set_cube_side_texture(_current_cube_id, 4, tex_id);
    set_cube_side_texture(_current_cube_id, 5, tex_id);
#endif
}


void iso_texture(int sheet_id, int ypos, int xpos)
{
#ifdef DC_CLIENT
    if(xpos <= 0 || ypos <= 0)
    {
        printf("Error: iso_texture index on block %i is less than zero! \n", _current_cube_id);
        abort();
    }
    xpos--;
    ypos--;

    //printf("Blit 1: %i %i %i \n", sheet_id, xpos, ypos);
    int tex_id = LUA_blit_cube_texture(sheet_id, xpos, ypos);
    //set cube side textures
    set_cube_side_texture(_current_cube_id, 0, tex_id);
    set_cube_side_texture(_current_cube_id, 1, tex_id);
    set_cube_side_texture(_current_cube_id, 2, tex_id);
    set_cube_side_texture(_current_cube_id, 3, tex_id);
    set_cube_side_texture(_current_cube_id, 4, tex_id);
    set_cube_side_texture(_current_cube_id, 5, tex_id);
#endif
}

void side_texture(int side, int tex_id)
{
#ifdef DC_CLIENT 
    set_cube_side_texture(_current_cube_id, side, tex_id);
#endif
}

void side_texture(int side, int sheet_id, int ypos, int xpos)
{
#ifdef DC_CLIENT
    if(xpos <= 0 || ypos <= 0)
    {
        printf("Block Dat Error: side_texture index on block %i is less than zero! \n", _current_cube_id);
        abort();
    }
    xpos--;
    ypos--;

    //printf("Blit 2: %i %i %i \n", sheet_id, xpos, ypos);
    int tex_id = LUA_blit_cube_texture(sheet_id, xpos, ypos);
    set_cube_side_texture(_current_cube_id, side, tex_id);
#endif
}

void palette_def(int palette_number)
{
#ifdef DC_CLIENT
    start_cube_palette(_current_cube_id);

    if( cube_texture_palette_lookup[_current_cube_id] + palette_number-1 != cube_texture_palette_index)
    {
        printf("start_palette_def error!!! mismatch on cube %i \n", _current_cube_id);
        printf("palette_lookup= %i palette_number= %i palette_index= %i \n", 
            cube_texture_palette_lookup[_current_cube_id],palette_number,cube_texture_palette_index  );
    }
#endif
}

void end_palette_def()
{
#ifdef DC_CLIENT
    end_cube_palette();
#endif
}

void palette_side_texture(int side, int sheet_id, int ypos, int xpos)
{
#ifdef DC_CLIENT
    if(xpos <= 0 || ypos <= 0)
    {
        printf("Block Dat Error: side_texture index on block %i is less than zero! \n", _current_cube_id);
        abort();
    }
    xpos--;
    ypos--;

    //printf("Blit 2: %i %i %i \n", sheet_id, xpos, ypos);
    int tex_id = LUA_blit_cube_texture(sheet_id, xpos, ypos);
    set_cube_palette_texture(_current_cube_id, side, tex_id);
#endif
}

void palette_side_texture(int side, int tex_id)
{
#ifdef DC_CLIENT
    set_cube_palette_texture(_current_cube_id, side, tex_id);
#endif
}

void palette_iso_texture(int sheet_id, int ypos, int xpos)
{
#ifdef DC_CLIENT
    if(xpos <= 0 || ypos <= 0)
    {
        printf("Error: iso_texture index on block %i is less than zero! \n", _current_cube_id);
        abort();
    }
    xpos--;
    ypos--;

    int tex_id = LUA_blit_cube_texture(sheet_id, xpos, ypos);
    //set cube side textures
    palette_side_texture(0, tex_id);
    palette_side_texture(1, tex_id);
    palette_side_texture(2, tex_id);
    palette_side_texture(3, tex_id);
    palette_side_texture(4, tex_id);
    palette_side_texture(5, tex_id);
#endif 
}

/*
void new_cube_palette(int cube_id);
void end_cube_palette();
void set_cube_palette_texture(int id, int palette, int side, int tex_id);
void set_cube_palette_color(int r, int g, int b, int a);
*/


void color_type(int color_type)
{
    LUA_set_block_color_type(_current_cube_id, color_type);
}

void hud_def(int hudy, int hudx, int tex_id)
{
#ifdef DC_CLIENT

    if(hudx <= 0 || hudy <= 0)
    {
        printf("Block Dat Error: hud_def1 index on block %i less than zero! \n", _current_cube_id);
        abort();
    }
    hudy--;
    hudx--;

    set_cube_hud(8*hudy+ hudx, _current_cube_id, tex_id);
#endif
}

void hud_def(int hudy,int hudx, int sheet_id, int ypos, int xpos)
{
#ifdef DC_CLIENT
    if(hudx <= 0 || hudy <= 0)
    {
        printf("Block Dat Error: hud_def2, hudx,hudy on block %i less than zero! \n", _current_cube_id);
        abort();
    }
    hudy--;
    hudx--;

    if(xpos <= 0 || ypos <= 0)
    {
        printf("Block Dat Error: hud_def2, xpos,ypos on block %i less than zero! \n", _current_cube_id);
        abort();
    }
    xpos--;
    ypos--;

    //printf("Blit 2: %i %i %i \n", sheet_id, xpos, ypos);
    int tex_id = LUA_blit_cube_texture(sheet_id, xpos, ypos);
    set_cube_hud(8*hudy+ hudx, _current_cube_id, tex_id);
#endif
}

void set_max_dmg(int dmg)
{
    LUA_set_block_max_damage(_current_cube_id, dmg);
}

int texture_alias(const char* spritesheet) 
{ 
#if DC_CLIENT
    return LUA_load_cube_texture_sheet((char*) spritesheet); 
#else
    return 0;
#endif
}

int sprite_alias(int sheet_id, int ypos, int xpos)
{
#if DC_CLIENT
    if(xpos <= 0 || ypos <= 0)
    {
        printf("Block Dat Error: hud_def2, xpos,ypos on block %i less than zero! \n", _current_cube_id);
        abort();
    }
    xpos--;
    ypos--;
    return LUA_blit_cube_texture(sheet_id, xpos, ypos); 
#else
    return 0;
#endif
}

void end_block_dat()
{
#if DC_CLIENT
    LUA_save_cube_texture();
#endif
}


/*
    TODO Write cleanup!!! of textures and FBO surfaces
*/
void blit_block_item_sheet()
{
#if DC_CLIENT

    GLuint block_item_64_texture = 0;
    {
        unsigned int color_tex, fb, depth_rb;
        const int xres = 1024;
        const int yres = 1024;
        const float scale = 64.0;

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
            break;
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
        for(int i=0; i<16; i++)
        for(int j=0; j<16; j++)
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
            int s1 = get_cube_side_texture(index, 0); //T
            int s2 = get_cube_side_texture(index, 2); //N
            int s3 = get_cube_side_texture(index, 4); //W
            
            draw_iso_cube(i*scale, j*scale, scale, s1,s2,s3);
        }
        glEnd();

        block_item_64_surface = create_surface_from_nothing(xres, yres);

        SDL_LockSurface(block_item_64_surface);
        glReadPixels(0, 0, xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, (void*) block_item_64_surface->pixels);
        SDL_UnlockSurface(block_item_64_surface);

        save_surface_to_png(block_item_64_surface, (char*)"screenshot/fbo_test_64.png");

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
        GS_ASSERT(block_item_64_texture != 0);
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
            break;
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

        //glBindTexture( GL_TEXTURE_2D, t_map::block_textures_normal);

        //glBegin(GL_QUADS);
        //for(int i=0; i<16; i++)
        //for(int j=0; j<16; j++)
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
        draw_bound_texture(0.0, 0.0, xres, yres);
        //glDisable(GL_TEXTURE_2D);

        block_item_16_surface = create_surface_from_nothing(xres, yres);

        SDL_LockSurface(block_item_16_surface);
        glReadPixels(0, 0, xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, (void*) block_item_16_surface->pixels);
        SDL_UnlockSurface(block_item_16_surface);

        save_surface_to_png(block_item_16_surface, (char*)"screenshot/fbo_test_16.png");

        //Delete resources
        glDeleteTextures(1, &color_tex);
        glDeleteRenderbuffersEXT(1, &depth_rb);
        //Bind 0, which means render to back buffer, as a result, fb is unbound
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glDeleteFramebuffersEXT(1, &fb);
    }
  
        glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
        glBindTexture( GL_TEXTURE_2D, 0);
        glViewport (0, 0, _xres, _yres);


#endif
}

}
