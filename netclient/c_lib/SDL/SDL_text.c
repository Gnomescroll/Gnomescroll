#include "SDL_text.h"

static GLuint fontTextureId;
static int tex_alpha = 1;

int _init_text() {

    SDL_Surface *font = IMG_Load("media/fonts/font.png");

    if(!font) { printf("SDL_text.init_test(): font load error, %s \n", IMG_GetError()); return 0;}
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
    return 0;

}

static const float dx = 1.0/16.0;
static const float dy = 1.0/16.0;

int _draw_text2(char* text, float x, float y, float height, float width, float depth, int r, int g, int b, int a) {

    int c_num = 0;

    int i;
    int j = 0;
    float offset = x;
    int index,xi,yi;
    float x_min, x_max, y_min, y_max;
    float Xmin,Xmax, Ymin,Ymax;

    while(text[c_num] != 0) { c_num++; }

    if (tex_alpha) {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        glColor4ub((unsigned char)r,(unsigned char)g,(unsigned char)b,(unsigned char)a); //replace with color cordinates on texture
    } else {
        glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b); //replace with color cordinates on texture
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, fontTextureId);

    for(i=0; i<c_num; i++){
        index = text[i];
        if (index == 10) {
            y -= 10;
            j = 0;
            continue;
        }
        xi = index % 16;
        yi = index >> 4;    // divides by 16 (n>>m == n/(2**m))
        //texture cordinates
        x_min = (float)(xi)*dx;
        x_max = (float)(xi+1)*dx;
        y_max = (float)(yi)*dy;
        y_min = (float)(yi+1)*dy;
        //vertex cordinates
        Xmin = j*width+x;
        Xmax = (j+1)*width+x;
        Ymin = y;
        Ymax = y+height;

        glBegin( GL_QUADS );
        glTexCoord2f(x_min,y_max);
        glVertex3f(Xmin, Ymax, depth);
        glTexCoord2f(x_min, y_min);
        glVertex3f(Xmin, Ymin, depth);
        glTexCoord2f(x_max, y_min);
        glVertex3f(Xmax, Ymin, depth);
        glTexCoord2f(x_max, y_max);
        glVertex3f(Xmax, Ymax, depth);
        glEnd();

        offset += width;
        j++;
    }
    glDisable(GL_TEXTURE_2D);
    if (tex_alpha) {
        glDisable(GL_BLEND);
    }
    return 0;
}



int _draw_text(char* text, float x, float y, float height, float width, float depth) {

int c_num = 0;

int i;
float offset = x;
int index,xi,yi;
float x_min, x_max, y_min, y_max;

float Xmin,Xmax, Ymin,Ymax;

while(text[c_num] != 0) { c_num++; }

    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, fontTextureId);
    for(i=0; i<c_num; i++){
        index = text[i];
        xi = index % 16;
        yi = index >> 4;
        //texture cordinates
        x_min = (float)(xi)*dx;
        x_max = (float)(xi+1)*dx;
        y_max = (float)(yi)*dy;
        y_min = (float)(yi+1)*dy;
        //vertex cordinates
        Xmin = i*width+x;
        Xmax = (i+1)*width+x;
        Ymin = y;
        Ymax = y+height;

        printf("xi, yi= %i, %i \n", xi, yi);
        printf("dx, dy: %f, %f \n", dx, dy);
        printf("x_min, x_max, y_min, y_max= %f, %f, %f, %f \n", x_min, x_max, y_min, y_max);
        printf("Xmin, Xmax, Ymin, Ymax= %f, %f, %f, %f \n", Xmin, Xmax, Ymin, Ymax);

        glBegin( GL_QUADS );
        glTexCoord2f(x_min,y_max);
        glVertex3f(Xmin, Ymax, depth);
        glTexCoord2f(x_min, y_min);
        glVertex3f(Xmin, Ymin, depth);
        glTexCoord2f(x_max, y_min);
        glVertex3f(Xmax, Ymin, depth);
        glTexCoord2f(x_max, y_max);
        glVertex3f(Xmax, Ymax, depth);
        glEnd();

        offset += width;
    }
    glDisable(GL_TEXTURE_2D);
    return 0;
}
