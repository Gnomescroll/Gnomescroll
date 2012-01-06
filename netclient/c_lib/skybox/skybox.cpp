#include "skybox.hpp"

#include <c_lib/camera/camera.hpp>
#include <c_lib/t_map/t_vbo.h>

#include <string.h>

/*
 * Adapted from:
 * http://sidvind.com/wiki/Skybox_tutorial
 *
 * Skybox textures generated according to:
 * https://developer.valvesoftware.com/wiki/Skybox_(2D)_with_Terragen
 */



namespace Skybox {

const int size = 1024;


static GLuint skybox_texture[6];

/*
USE ORDER
GL_TEXTURE_CUBE_MAP_POSITIVE_X
GL_TEXTURE_CUBE_MAP_NEGATIVE_X
GL_TEXTURE_CUBE_MAP_POSITIVE_Y
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
GL_TEXTURE_CUBE_MAP_POSITIVE_Z
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z

*/

void load() {


    load_skybox_cubemap();
    return;


    SDL_Surface* surface;
    const char fmt_str[] = "media/skybox/skyrender%04d.png";
    char filename[strlen(fmt_str)+1];
    int i;
    int tex_format;

    glEnable(GL_TEXTURE_2D);
    
    for (i=0; i<6; i++) {
        sprintf(filename, fmt_str, i+1);
        surface = IMG_Load(filename);
        if (surface == NULL || !surface) {
            printf("IMG_Load: %s \n", IMG_GetError());
            continue;
        }

        if (surface->format->Rmask == 0x000000ff)
            tex_format = GL_RGBA;
        else
            tex_format = GL_BGRA;

        glGenTextures(1, &skybox_texture[i]);
        glBindTexture(GL_TEXTURE_2D, skybox_texture[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
        glTexImage2D(
            GL_TEXTURE_2D,  //target
            0,  // level
            4,  // internalformat, use 4 for 4 bytes per pixel
            surface->w, surface->h, //width, height
            0,  // border
            tex_format, // format
            GL_UNSIGNED_BYTE,   // type
            surface->pixels // pixels
        );
    
        SDL_FreeSurface(surface);
    }

    glDisable(GL_TEXTURE_2D);
    printf("Loaded skybox textures\n");
}

/*  Render a skybox with center point position and dimension sizes size */
/* Adapted from
 * http://content.gpwiki.org/index.php/Sky_Box
 */
void render() {

    draw_skybox_cubemap();
    return;

// Begin DrawSkybox
    glColor4f(1.0, 1.0, 1.0,1.0f);

// Save Current Matrix
    glPushMatrix();

// Enable/Disable features
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    //glDisable(GL_LIGHTING);
    //glDisable(GL_BLEND);

// Second Move the render space to the correct position (Translate)
    //glTranslatef(position.x,position.y,position.z);
    glTranslatef(current_camera->x, current_camera->y, current_camera->z);

// First apply scale matrix
    //glScalef(size, size, size);  // a big number?
    
    //float _sky_scale = 1.0;
    //glScalef(_sky_scale, _sky_scale, _sky_scale);

    float cz = 0.0f,cx = 1.0f; // texture corners
    float r = 1.005f; // If you have border issues change this to 1.005f
    // increasing it further does not eliminate remaining border problems & warps the skybox to bad statezzzzzzzzzz
    float y = 1.0f;//dont change this

//F L U B R D
// Common Axis Z - FRONT Side
    glBindTexture(GL_TEXTURE_2D, skybox_texture[0]);
    glBegin(GL_QUADS);
    glTexCoord2f(cx, cx);
    glVertex3f(-r ,y,-r);
    glTexCoord2f(cz, cx);
    glVertex3f( r ,y,-r);
    glTexCoord2f(cz, cz);
    glVertex3f( r,y,r);
    glTexCoord2f(cx, cz);
    glVertex3f(-r,y,r);
    glEnd();

// Common Axis Z - BACK side
    glBindTexture(GL_TEXTURE_2D, skybox_texture[3]);
    glBegin(GL_QUADS);
    
    glTexCoord2f(cx,cx);
    glVertex3f( r,-y,-r);

    glTexCoord2f(cz,cx);
    glVertex3f(-r,-y,-r);

    glTexCoord2f(cz,cz);
    glVertex3f(-r,-y, r);

    glTexCoord2f(cx,cz);
    glVertex3f( r,-y, r);
    glEnd();

// Common Axis X - Left side
    glBindTexture(GL_TEXTURE_2D,skybox_texture[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(cx,cx);
    glVertex3f(-y, -r,-r);
    glTexCoord2f(cz,cx);
    glVertex3f(-y, r,-r);
    glTexCoord2f(cz,cz);
    glVertex3f(-y, r, r);
    glTexCoord2f(cx,cz);
    glVertex3f(-y, -r, r);
    glEnd();

// Common Axis X - Right side
    glBindTexture(GL_TEXTURE_2D,skybox_texture[4]);
    glBegin(GL_QUADS);
    glTexCoord2f( cx,cx);
    glVertex3f(y, r,-r);
    glTexCoord2f(cz, cx);
    glVertex3f(y, -r,-r);
    glTexCoord2f(cz, cz);
    glVertex3f(y, -r, r);
    glTexCoord2f(cx, cz);
    glVertex3f(y, r, r);
    glEnd();

// Common Axis Y - Draw Up side
    glBindTexture(GL_TEXTURE_2D,skybox_texture[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(cz, cz);
    glVertex3f(-r, -r,y);
    glTexCoord2f(cx, cz);
    glVertex3f(-r, r,y);
    glTexCoord2f(cx, cx);
    glVertex3f( r, r,y);
    glTexCoord2f(cz, cx);
    glVertex3f( r, -r,y);
    glEnd();

// Common Axis Y - Down side
    glBindTexture(GL_TEXTURE_2D,skybox_texture[5]);
    glBegin(GL_QUADS);
    glTexCoord2f(cz, cz);
    glVertex3f( r, -r,-y);
    glTexCoord2f(cx, cz);
    glVertex3f( r, r,-y);
    glTexCoord2f(cx, cx);
    glVertex3f(-r, r,-y);
    glTexCoord2f(cz, cx);
    glVertex3f(-r, -r,-y);
    glEnd();

// Load Saved Matrix
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

}

static GLuint skybox_cubemap;

void load_skybox_cubemap() {

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); //enable once and dont change

    SDL_Surface* surface;
    const char fmt_str[] = "media/skybox/skyrender%04d.png";
    char filename[strlen(fmt_str)+1];
    int tex_format;

    //glEnable(GL_TEXTURE_2D);


    glGenTextures(1, &skybox_cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_cubemap);

    for (int i = 0; i < 6; i++) 
    {

        sprintf(filename, fmt_str, i+1);
        surface = IMG_Load(filename);
        if (surface == NULL || !surface) {
            printf("IMG_Load: %s \n", IMG_GetError());
            continue;
        }

        if (surface->format->Rmask == 0x000000ff)
            tex_format = GL_RGBA;
        else
            tex_format = GL_BGRA;
            
        //
    /*
        if (info->format == GL_RGBA8)
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, 
                info->format, 
                info->width, 
                info->height, 
                0, GL_RGBA, 
                GL_UNSIGNED_BYTE, 
                data + info->ofsCMSides[i]
            );
    */
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,  //target
            0,  // level
            4,  // internalformat, use 4 for 4 bytes per pixel
            surface->w, surface->h, //width, height
            0,  // border
            tex_format, // format
            GL_UNSIGNED_BYTE,   // type
            surface->pixels // pixels
        );
        
        SDL_FreeSurface(surface);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

/*
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
*/
}
/*
    //glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    for (i=0; i<6; i++) {
        sprintf(filename, fmt_str, i+1);
        surface = IMG_Load(filename);
        if (surface == NULL || !surface) {
            printf("IMG_Load: %s \n", IMG_GetError());
            continue;
        }

        if (surface->format->Rmask == 0x000000ff)
            tex_format = GL_RGBA;
        else
            tex_format = GL_BGRA;

        //glGenTextures(1, &skybox_texture[i]);
        glBindTexture(GL_TEXTURE_2D, skybox_texture[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
        glTexImage2D(
            GL_TEXTURE_2D,  //target
            0,  // level
            4,  // internalformat, use 4 for 4 bytes per pixel
            surface->w, surface->h, //width, height
            0,  // border
            tex_format, // format
            GL_UNSIGNED_BYTE,   // type
            surface->pixels // pixels
        );
    
        SDL_FreeSurface(surface);
    }

    glDisable(GL_TEXTURE_2D);
    printf("Loaded skybox textures\n");
*/

void draw_skybox_cubemap()
{



// Begin DrawSkybox
    glColor4f(1.0, 1.0, 1.0,1.0f);

// Save Current Matrix
    glPushMatrix();

// Enable/Disable features
    glPushAttrib(GL_ENABLE_BIT);
    //glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    //glDisable(GL_LIGHTING);
    //glDisable(GL_BLEND);

    glEnable(GL_TEXTURE_CUBE_MAP);


// Second Move the render space to the correct position (Translate)
    //glTranslatef(position.x,position.y,position.z);
    glTranslatef(current_camera->x, current_camera->y, current_camera->z);

// First apply scale matrix
    //glScalef(size, size, size);  // a big number?
    
    //float _sky_scale = 1.0;
    //glScalef(_sky_scale, _sky_scale, _sky_scale);

    const float cz = 0.0f;  // texture corners
    const float cx = 1.0f;

    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_cubemap);



    //const float SQRT_3_3 = 1.0f;
/*
    const static float ve[24] = 
    {  // the idea is that all vectors have a length of 1 so that position can also be used as cubemap texture coords
        -SQRT_3_3,  -SQRT_3_3,  -SQRT_3_3,
        SQRT_3_3,   -SQRT_3_3,  -SQRT_3_3,
        -SQRT_3_3,  SQRT_3_3,   -SQRT_3_3,
        SQRT_3_3,   SQRT_3_3,   -SQRT_3_3,
        -SQRT_3_3,  -SQRT_3_3,  SQRT_3_3,
        SQRT_3_3,   -SQRT_3_3,  SQRT_3_3,
        -SQRT_3_3,  SQRT_3_3,   SQRT_3_3,
        SQRT_3_3,   SQRT_3_3,   SQRT_3_3
    };

    const static int in[24] = 
    {
        1,          5,          7,          3,  // positive x
        2,          0,          4,          6,  // negative x
        4,          5,          7,          6,  // positive y
        0,          1,          3,          2,  // negative y
        0,          1,          5,          4,  // positive z
        3,          2,          6,          7   // negative z
    };
*/

    const float r = 1.000f; // If you have border issues change this to 1.005f
    // increasing it further does not eliminate remaining border problems & warps the skybox to bad statezzzzzzzzzz
    const float y = 1.0f;//dont change this

/*
const static float ve[4*3*6] = {
    -r,y,-r,
    r,y,-r,
    r,y,r,
    -r,y,r,

    r,-y,-r,
    -r,-y,-r,
    -r,-y, r,
    r,-y, r,

    -y, -r,-r,
    -y, r,-r,
    -y, r, r,
    -y, -r, r,

    y, r,-r,
    y, -r,-r,
    y, -r, r,
    y, r, r,

    -r, -r,y,
    -r, r,y,
    r, r,y,
    r, -r,y,

    r, -r,-y,
    r, r,-y,
    -r, r,-y,
    -r, -r,-y,
};
*/


    float i_f;

/*
USE ORDER
GL_TEXTURE_CUBE_MAP_POSITIVE_X
GL_TEXTURE_CUBE_MAP_NEGATIVE_X
GL_TEXTURE_CUBE_MAP_POSITIVE_Y
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
GL_TEXTURE_CUBE_MAP_POSITIVE_Z
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
*/

glBegin(GL_QUADS);

// +X
    i_f = 0.0;
    glTexCoord3f(cx,cx,i_f);
    glVertex3f(y, r,-r);
    glTexCoord3f(cz,cx,i_f);
    glVertex3f(y, -r,-r);
    glTexCoord3f(cz,cz,i_f);
    glVertex3f(y, -r, r);
    glTexCoord3f(cx,cz,i_f);
    glVertex3f(y, r, r);

// -X
    i_f = 1.0;
    glTexCoord3f(cx,cx,i_f);
    glVertex3f(-y, -r,-r);
    glTexCoord3f(cz,cx,i_f);
    glVertex3f(-y, r,-r);
    glTexCoord3f(cz,cz,i_f);
    glVertex3f(-y, r, r);
    glTexCoord3f(cx,cz,i_f);
    glVertex3f(-y, -r, r);

// +Y
    i_f = 2.0;
    glTexCoord3f(cx,cx,i_f);
    glVertex3f(-r ,y,-r);
    glTexCoord3f(cz,cx,i_f);
    glVertex3f( r ,y,-r);
    glTexCoord3f(cz,cz,i_f);
    glVertex3f( r,y,r);
    glTexCoord3f(cx,cz,i_f);
    glVertex3f(-r,y,r);

// -Y
    i_f = 3.0;
    glTexCoord3f(cx,cx,i_f);
    glVertex3f( r,-y,-r);
    glTexCoord3f(cz,cx,i_f);
    glVertex3f(-r,-y,-r);
    glTexCoord3f(cz,cz,i_f);
    glVertex3f(-r,-y, r);
    glTexCoord3f(cx,cz,i_f);
    glVertex3f( r,-y, r);

// +Z
    i_f = 4.0;
    glTexCoord3f(cx,cx,i_f);
    glVertex3f(-r, -r,y);
    glTexCoord3f(cz,cx,i_f);
    glVertex3f(-r, r,y);
    glTexCoord3f(cz,cz,i_f);
    glVertex3f( r, r,y);
    glTexCoord3f(cx,cz,i_f);
    glVertex3f( r, -r,y);

// -Z
    i_f = 5.0;
    glTexCoord3f(cx,cx,i_f);
    glVertex3f( r, -r,-y);
    glTexCoord3f(cz,cx,i_f);
    glVertex3f( r, r,-y);
    glTexCoord3f(cz,cz,i_f);
    glVertex3f(-r, r,-y);
    glTexCoord3f(cx,cz,i_f);
    glVertex3f(-r, -r,-y);

glEnd();

/*
    glBegin(GL_QUADS);
    for(int i=0; i < 6; i++)
    {
        glTexCoord3f(cx, cx, i_f);

        glVertex3f(ve[12*i+3*0+0], ve[12*i+3*0+1], ve[12*i+3*0+2] );
     
        glTexCoord3f(cz, cx, i_f);

        glVertex3f(ve[12*i+3*1+0], ve[12*i+3*1+1], ve[12*i+3*1+2] );

        glTexCoord3f(cz, cz, i_f);

        glVertex3f(ve[12*i+3*2+0], ve[12*i+3*2+1], ve[12*i+3*2+2] );

        glTexCoord3f(cx, cz, i_f);

        glVertex3f(ve[12*i+3*3+0], ve[12*i+3*3+1], ve[12*i+3*3+2] );

        i_f += 1.0f;
    }
    glEnd();
*/


/*
    glBegin(GL_QUADS);
    for(int i=0; i < 6; i++)
    {
        glTexCoord3f(cx, cx, i_f);

        glVertex3f(ve[3*in[4*i+0]+0], ve[3*in[4*i+0]+1], ve[3*in[4*i+0]+2] );
     
        glTexCoord3f(cz, cx, i_f);

        glVertex3f(ve[3*in[4*i+1]+0], ve[3*in[4*i+1]+1], ve[3*in[4*i+1]+2] );

        glTexCoord3f(cz, cz, i_f);

        glVertex3f(ve[3*in[4*i+2]+0], ve[3*in[4*i+2]+1], ve[3*in[4*i+2]+2] );

        glTexCoord3f(cx, cz, i_f);

        glVertex3f(ve[3*in[4*i+3]+0], ve[3*in[4*i+3]+1], ve[3*in[4*i+3]+2] );

        i_f += 1.0f;
    }
    glEnd();
*/


    glDisable(GL_TEXTURE_CUBE_MAP);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

}


};
