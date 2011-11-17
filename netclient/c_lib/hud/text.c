#include "text.h"

static GLuint fontTextureId;
static int tex_alpha = 1;
static int font_loaded = 0;

int load_font(char* fontfile) {
    SDL_Surface *font = IMG_Load(fontfile);

    if(!font) { printf("text.init_test(): font load error, %s \n", IMG_GetError()); return 0;}
    if(font->format->BytesPerPixel != 4) {
        printf("Font Image File: image is missing alpha channel \n");
        tex_alpha = 0;
    }

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1,&fontTextureId);
    glBindTexture(GL_TEXTURE_2D,fontTextureId);

    if (tex_alpha) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font->w, font->h, 0, GL_RGBA, //rgb
                     GL_UNSIGNED_BYTE, font->pixels);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, font->w, font->h, 0, GL_RGB, //rgb
                     GL_UNSIGNED_BYTE, font->pixels);
    }

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    SDL_FreeSurface(font);

    glDisable(GL_TEXTURE_2D);
    font_loaded = 1;
    printf("Loaded font %s\n", fontfile);
    return 1;
}

void start_text_draw(int r, int g, int b, int a) {
    
    if (!font_loaded) {
        printf("No font loaded\n");
        return;
    }

    if (tex_alpha) {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        glColor4ub((unsigned char)r,(unsigned char)g,(unsigned char)b,(unsigned char)a); //replace with color cordinates on texture
    } else {
        glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b); //replace with color cordinates on texture
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fontTextureId);
    glBegin( GL_QUADS );
}

void blit_glyph(
    float tex_x_min, float tex_x_max,
    float tex_y_min, float tex_y_max,
    float screen_x_min, float screen_x_max,
    float screen_y_min, float screen_y_max,
    float depth)
{
    glTexCoord2f(tex_x_min, tex_y_max);
    glVertex3f(screen_x_min, screen_y_max, depth);
    glTexCoord2f(tex_x_min, tex_y_min);
    glVertex3f(screen_x_min, screen_y_min, depth);
    glTexCoord2f(tex_x_max, tex_y_min);
    glVertex3f(screen_x_max, screen_y_min, depth);
    glTexCoord2f(tex_x_max, tex_y_max);
    glVertex3f(screen_x_max, screen_y_max, depth);
}

void end_text_draw() {
    glEnd();
    glDisable(GL_TEXTURE_2D);
    if (tex_alpha) {
        glDisable(GL_BLEND);
    }
}
