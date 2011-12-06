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

static GLuint skybox_texture[6];

const int size = 1024;

void load() {

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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
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
// Begin DrawSkybox
    glColor4f(1.0, 1.0, 1.0,1.0f);

// Save Current Matrix
    glPushMatrix();

// Enable/Disable features
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);

// Second Move the render space to the correct position (Translate)
    //glTranslatef(position.x,position.y,position.z);
    glTranslatef(current_camera->x, current_camera->y, current_camera->z);

// First apply scale matrix
    glScalef(size, size, size);  // a big number?

    float cz = -0.0f,cx = 1.0f; // texture corners
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

};
