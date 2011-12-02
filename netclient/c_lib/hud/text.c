#include "text.h"

GLuint fontTextureId;
static int tex_alpha = 1;
static int font_loaded = 0;

void init_glyphs();

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

    init_glyphs();
    
    return 1;
}

void set_text_color(int r, int g, int b, int a) {
    if (tex_alpha) {
        glColor4ub((unsigned char)r,(unsigned char)g,(unsigned char)b,(unsigned char)a); //replace with color cordinates on texture
    } else {
        glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b); //replace with color cordinates on texture
    }
}
    
void start_text_draw() {
    
    if (!font_loaded) {
        printf("No font loaded\n");
        return;
    }

    if (tex_alpha) {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fontTextureId);
    glBegin(GL_QUADS);
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


/* Glyph store */

struct Glyph glyphs[128];
int missing_character = 63; // "?"

void set_missing_character(int cc) {
    missing_character = cc;
}

void init_glyphs() {
    static int once = 0;
    if (once) return;
    int i;
    for (i=0; i<128; i++) {
        glyphs[i].x = 0.0f;
        glyphs[i].y = 0.0f;
        glyphs[i].xoff = 0.0f;
        glyphs[i].yoff = 0.0f;
        glyphs[i].w = 0.0f;
        glyphs[i].h = 0.0f;
        glyphs[i].xadvance = 0.0f;
        glyphs[i].available = 0;
    }
    once++;
}

void add_glyph(
    int c,
    float x, float y,
    float xoff, float yoff,
    float w, float h,
    float xadvance
) {
    glyphs[c].x = x / 256.0f;
    glyphs[c].y = y / 256.0f;
    glyphs[c].tw = w / 256.0f;
    glyphs[c].th = h / 256.0f;
    glyphs[c].w = w;
    glyphs[c].h = h;
    glyphs[c].xoff = xoff;
    glyphs[c].yoff = yoff;
    glyphs[c].xadvance = xadvance;
    glyphs[c].available = 1;
}

void draw_text(char* t, int len, float x, float y, float depth) {
    int i;
    //char c;
    int c;
    struct Glyph glyph;

    float tx_min, tx_max, ty_min, ty_max;
    float sx_min, sx_max, sy_min, sy_max;
    float cursor_x = 0.0f;
    float cursor_y = 0.0f;

    for (i=0; i<len; i++) {
        c = t[i];

        // newline
        if (c == 10) {
            cursor_y += 10.0f;
            continue;
        }
        
        glyph = glyphs[c];

        // known glyph?
        if (! glyph.available) {
            printf("Character unknown: %c\n", c);
            glyph = glyphs[missing_character];
        }

        tx_max = glyph.x;
        tx_min = glyph.x + glyph.tw;
        ty_min = glyph.y;
        ty_max = glyph.y + glyph.th;

        sx_max = x + cursor_x + glyph.xoff;
        sx_min = x + cursor_x + glyph.xoff + glyph.w;
        sy_min = y - (cursor_y + glyph.yoff);
        sy_max = y - (cursor_y + glyph.yoff + glyph.h);
        blit_glyph(tx_min, tx_max, ty_min, ty_max, sx_min, sx_max, sy_min, sy_max, depth);

        cursor_x += glyph.xadvance;
    }
}
