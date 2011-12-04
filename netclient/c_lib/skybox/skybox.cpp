#include "skybox.hpp"

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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(
            GL_TEXTURE_2D,  //target
            0,  // level
            tex_format,  // internalformat
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

/*
 * As explained above this makes sure the textures displayed correctly and
 * no edges will appear on the quads. In some cases we need to turn off
 * interpolation too. Just set GL_TEXTURE_MAG_FILTER and GL_TEXTURE_MIN_FILTER
 * to GL_NEAREST. Below is a sample routine to render the skybox. All lines
 * will be explained later.
 */

void render() {
//Code: Rendering
    // Store the current matrix
    glPushMatrix();
 
    // Reset and transform the matrix.
    glLoadIdentity();
    gluLookAt(
        0,0,0,
        current_camera->x,// + current_camera->xl,
        current_camera->y,// + current_camera->yl,
        current_camera->z,// + current_camera->zl,
        0,1,0);
 
    // Enable/Disable features
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
 
    // Just in case we set all vertices to white.
    glColor4f(1,1,1,1);
 
    // Render the front quad
    glBindTexture(GL_TEXTURE_2D, skybox_texture[0]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(  0.5f, -0.5f, -0.5f );
        glTexCoord2f(1, 0); glVertex3f( -0.5f, -0.5f, -0.5f );
        glTexCoord2f(1, 1); glVertex3f( -0.5f,  0.5f, -0.5f );
        glTexCoord2f(0, 1); glVertex3f(  0.5f,  0.5f, -0.5f );
    glEnd();
 
    // Render the left quad
    glBindTexture(GL_TEXTURE_2D, skybox_texture[1]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(  0.5f, -0.5f,  0.5f );
        glTexCoord2f(1, 0); glVertex3f(  0.5f, -0.5f, -0.5f );
        glTexCoord2f(1, 1); glVertex3f(  0.5f,  0.5f, -0.5f );
        glTexCoord2f(0, 1); glVertex3f(  0.5f,  0.5f,  0.5f );
    glEnd();
 
    // Render the back quad
    glBindTexture(GL_TEXTURE_2D, skybox_texture[3]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -0.5f, -0.5f,  0.5f );
        glTexCoord2f(1, 0); glVertex3f(  0.5f, -0.5f,  0.5f );
        glTexCoord2f(1, 1); glVertex3f(  0.5f,  0.5f,  0.5f );
        glTexCoord2f(0, 1); glVertex3f( -0.5f,  0.5f,  0.5f );
 
    glEnd();
 
    // Render the right quad
    glBindTexture(GL_TEXTURE_2D, skybox_texture[4]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -0.5f, -0.5f, -0.5f );
        glTexCoord2f(1, 0); glVertex3f( -0.5f, -0.5f,  0.5f );
        glTexCoord2f(1, 1); glVertex3f( -0.5f,  0.5f,  0.5f );
        glTexCoord2f(0, 1); glVertex3f( -0.5f,  0.5f, -0.5f );
    glEnd();
 
    // Render the top quad
    glBindTexture(GL_TEXTURE_2D, skybox_texture[2]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex3f( -0.5f,  0.5f, -0.5f );
        glTexCoord2f(0, 0); glVertex3f( -0.5f,  0.5f,  0.5f );
        glTexCoord2f(1, 0); glVertex3f(  0.5f,  0.5f,  0.5f );
        glTexCoord2f(1, 1); glVertex3f(  0.5f,  0.5f, -0.5f );
    glEnd();
 
    // Render the bottom quad
    glBindTexture(GL_TEXTURE_2D, skybox_texture[5]);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -0.5f, -0.5f, -0.5f );
        glTexCoord2f(0, 1); glVertex3f( -0.5f, -0.5f,  0.5f );
        glTexCoord2f(1, 1); glVertex3f(  0.5f, -0.5f,  0.5f );
        glTexCoord2f(1, 0); glVertex3f(  0.5f, -0.5f, -0.5f );
    glEnd();
 
    // Restore enable bits and matrix
    glPopAttrib();
    glPopMatrix();
}

};
