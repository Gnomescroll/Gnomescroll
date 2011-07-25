#include "SDL_text.h"


TTF_Font *font;

GLuint fontTextureId;

char* fontpath = "media/fonts/freesansbold.ttf";

int _init_text() {
    TTF_Init();
    //atexit(TTF_Quit);

/*
    if(!(font = TTF_OpenFont(fontpath, 20))) {
        printf("Error loading font: %s", TTF_GetError());
        return 1;
    }
    return 0;
*/
    SDL_Surface *font = IMG_Load("media/fonts/font.png");

    if(!font) { printf("SDL_text.init_test(): font load error, %s \n", IMG_GetError()); return 0;}
    if(font->format->BytesPerPixel != 4) {printf("Font Image File: image is missing alpha channel \n");}

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1,&fontTextureId);
    glBindTexture(GL_TEXTURE_2D,fontTextureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font->w, font->h, 0, GL_RGB, //rgb
                 GL_UNSIGNED_BYTE, font->pixels);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
/*
    float cx;
    float cy;

    base = glGenLists(256);

    for(int i = 0; i < 256; i++)
    {
        cx = float(i%16)/16.0f;
        cy = float(i/16)/16.0f;

        glNewList(base+i,GL_COMPILE);
        glBegin(GL_QUADS);
            glTexCoord2f(cx,cy+0.0625f);
            glVertex2i(0,16);
            glTexCoord2f(cx+0.0625f,cy+0.0625f);
            glVertex2i(16,16);
            glTexCoord2f(cx+0.0625f,cy);
            glVertex2i(16,0);
            glTexCoord2f(cx,cy);
            glVertex2i(0,0);
        glEnd();
        glTranslated(10,0,0);
        glEndList();
    }
    glDisable(GL_TEXTURE_2D);
*/
    SDL_FreeSurface(font);

    glDisable(GL_TEXTURE_2D);

}

const float dx = 1.0/16.0;
const float dy = 1.0/16.0;

int _draw_text2(char* text, float x, float y, float height, float width, float depth, int r, int g, int b, int a) {

int c_num = 0;

int i;
int j = 0;
float offset = x;
int index,xi,yi;
float x_min, x_max, y_min, y_max;
float xmin,xmax, ymin,ymax;

while(text[c_num] != 0) { c_num++; }

    glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b); //replace with color cordinates on texture
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
        xmin = j*width+x;
        xmax = (j+1)*width+x;
        ymin = y;
        ymax = y+height;
/*
        printf("xi, yi= %i, %i \n", xi, yi);
        printf("dx, dy: %f, %f \n", dx, dy);
        printf("x_min, x_max, y_min, y_max= %f, %f, %f, %f \n", x_min, x_max, y_min, y_max);
        printf("xmin, xmax, ymin, ymax= %f, %f, %f, %f \n", xmin, xmax, ymin, ymax);
*/

        glBegin( GL_QUADS );
            glTexCoord2f(x_min,y_max);
            glVertex3f(xmin, ymax, depth);
            glTexCoord2f(x_min, y_min);
            glVertex3f(xmin, ymin, depth);
            glTexCoord2f(x_max, y_min);
            glVertex3f(xmax, ymin, depth);
            glTexCoord2f(x_max, y_max);
            glVertex3f(xmax, ymax, depth);
        glEnd();

        offset += width;
        j++;
    }
    glDisable(GL_TEXTURE_2D);
}



int _draw_text(char* text, float x, float y, float height, float width, float depth) {
//float width = 40.0;
//float height = 40.0;
//float depth = -0.5;

int c_num = 0;
while(text[c_num] != 0) { c_num++; }

int i;
float offset = x;
int index,xi,yi;
float x_min, x_max, y_min, y_max;

float xmin,xmax, ymin,ymax;

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
        xmin = i*width+x;
        xmax = (i+1)*width+x;
        ymin = y;
        ymax = y+height;

        printf("xi, yi= %i, %i \n", xi, yi);
        printf("dx, dy: %f, %f \n", dx, dy);
        printf("x_min, x_max, y_min, y_max= %f, %f, %f, %f \n", x_min, x_max, y_min, y_max);
        printf("xmin, xmax, ymin, ymax= %f, %f, %f, %f \n", xmin, xmax, ymin, ymax);

/*
        glColor3ub(255,255,255);
        glBegin(GL_LINES);
        glVertex3f(xmin,ymin,-0.6); // origin of the line
        glVertex3f(xmax, ymax,-0.6); // ending point of the line
        glEnd();
*/
        glBegin( GL_QUADS );
            glTexCoord2f(x_min,y_max);
            glVertex3f(xmin, ymax, depth);
            glTexCoord2f(x_min, y_min);
            glVertex3f(xmin, ymin, depth);
            glTexCoord2f(x_max, y_min);
            glVertex3f(xmax, ymin, depth);
            glTexCoord2f(x_max, y_max);
            glVertex3f(xmax, ymax, depth);
        glEnd();

        offset += width;
    }
    glDisable(GL_TEXTURE_2D);
}

/// DEPRECATE BELOW LINE
//SDL_FreeSurface
enum textquality {solid, shaded, blended};

SDL_Surface *drawtext(TTF_Font *fonttodraw, char fgR, char fgG, char fgB, char fgA,
char bgR, char bgG, char bgB, char bgA, char text[], int quality)
{
  SDL_Color tmpfontcolor = {fgR,fgG,fgB,fgA};
  SDL_Color tmpfontbgcolor = {bgR, bgG, bgB, bgA};
  SDL_Surface *resulting_text;

  if (quality == solid) resulting_text = TTF_RenderText_Solid(fonttodraw, text, tmpfontcolor);
  else if (quality == shaded) resulting_text = TTF_RenderText_Shaded(fonttodraw, text, tmpfontcolor, tmpfontbgcolor);
  else if (quality == blended) resulting_text = TTF_RenderText_Blended(fonttodraw, text, tmpfontcolor);

  return resulting_text;
}

SDL_Surface* _create_text_surface(char* text, int r, int g, int b, int a){
    SDL_Color tmpfontcolor = {r,g,b,a};
    return TTF_RenderText_Blended(font, text, tmpfontcolor);
}

int _draw_text_surface(SDL_Surface* surface, int x, int y){

    return 0;
}


int _free_text_surface(SDL_Surface* surface) { SDL_FreeSurface(surface); return 0;}

