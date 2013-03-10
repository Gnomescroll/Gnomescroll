#pragma once


namespace Hud
{


class HudRadiationMeter
{
    public:
        struct SDL_Surface* surface;
        struct SDL_Surface* border_surface;
        struct SDL_Surface* gradient_surface;

        GLuint tex;
        GLuint border_tex;
        GLuint gradient_tex;

        static const int width = 64;

    HudRadiationMeter() :
        surface(NULL), border_surface(NULL), gradient_surface(NULL),
        tex(0), border_tex(0), gradient_tex(0)
    {
    }

    ~HudRadiationMeter()
    {
        if (this->surface != NULL)
            SDL_FreeSurface(this->surface);
        if (this->border_surface != NULL)
            SDL_FreeSurface(this->border_surface);
        if (this->gradient_surface != NULL)
            SDL_FreeSurface(this->gradient_surface);
    }

    void init()
    {
        GS_ASSERT(surface == NULL);
        GS_ASSERT(border_surface == NULL);
        GS_ASSERT(gradient_surface == NULL);

        surface = create_surface_from_file(MEDIA_PATH "sprites/icons/radiation_hud.png");
        border_surface = create_surface_from_file(MEDIA_PATH "sprites/icons/radiation_border.png");
        gradient_surface = create_surface_from_file(MEDIA_PATH "sprites/gradient/heightmap_gradient_01.png");
        IF_ASSERT(surface == NULL ||
                  border_surface == NULL ||
                  gradient_surface == NULL) return;

        GLenum tex_format = get_texture_format(surface);
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, tex_format, GL_UNSIGNED_BYTE, surface->pixels);
        glDisable(GL_TEXTURE_2D);

        tex_format = get_texture_format(border_surface);
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &border_tex);
        glBindTexture(GL_TEXTURE_2D, border_tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, border_surface->w, border_surface->h, 0, tex_format, GL_UNSIGNED_BYTE, border_surface->pixels);
        glDisable(GL_TEXTURE_2D);

        tex_format = get_texture_format(gradient_surface);
        glDisable(GL_TEXTURE_1D);
        glGenTextures(1, &gradient_tex);
        glBindTexture(GL_TEXTURE_1D, gradient_tex);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, gradient_surface->w, 0, tex_format, GL_UNSIGNED_BYTE, gradient_surface->pixels);

        glDisable(GL_TEXTURE_1D);
    }

    void draw_circle(int x, int y, int degree)
    {
        const float radius = this->width/2.0f;
        const float fiddle_factor = -6.0f;

        int max = 16;
        const float z = -0.1f;

        //float _max = 1.0 / ((float) max);
        degree = GS_MIN(degree, max);

        glColor4ub(255, 0, 0, 255);

        glBegin(GL_TRIANGLES);

        float xf = x + width/2.0f;
        float yf = y + width/2.0f;

        for(int i=0; i<degree; i++)
        {
            float frac1 = i / float(max);
            float x1 = xf +radius*sinf(2*PI*frac1);
            float y1 = yf +radius*cosf(2*PI*frac1);

            float frac2 = (i + 0.5f) / float(max);
            float x2 = xf + (fiddle_factor+radius)*sinf(2*PI*frac2);
            float y2 = yf + (fiddle_factor+radius)*cosf(2*PI*frac2);

            float frac3 = (i + 1.0f) / float(max);
            float x3 = xf + radius*sinf(2*PI*frac3);
            float y3 = yf + radius*cosf(2*PI*frac3);

            glVertex3f(x1, y1, z);
            glVertex3f(x2, y2, z);
            glVertex3f(x3, y3, z);
        }

        glEnd();
    }

    void draw(float x, float y)
    {
        IF_ASSERT(gradient_surface == NULL || gradient_surface->w == 0) return;
        static int c = 0;
        int i = (c++) % gradient_surface->w;
        int r = ((unsigned char*)gradient_surface->pixels)[4*i + 0];
        int g = ((unsigned char*)gradient_surface->pixels)[4*i + 1];
        int b = ((unsigned char*)gradient_surface->pixels)[4*i + 2];

        float x0 = x;
        float x1 = x + width;
        float y0 = y;
        float y1 = y + width;
        const float z = -0.1f;
        const float blur = 1.0f;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_2D);

        glColor4ub(r, g, b, 0x80);

        // draw border
        glBindTexture(GL_TEXTURE_2D, border_tex);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(x0, y0, z);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(x1, y0, z);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(x1, y1, z);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(x0, y1, z);
        glEnd();

        // draw inner area
        glBindTexture(GL_TEXTURE_2D, tex);
        glColor4ub(r, g, b, 0x60);
        glBegin(GL_QUADS);
        for (int i=0; i<4; i++)
        {
            int j = i % 2;
            int k = (i + 1) % 2;
            float dx = j * blur;
            float dy = k * blur;
            dx *= (j == 0) ? -1 : 1;
            dy *= (k == 0) ? -1 : 1;

            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(x0+dx, y0+dy, z);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(x1+dx, y0+dy, z);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(x1+dx, y1+dy, z);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(x0+dx, y1+dy, z);
        }
        glEnd();

        glColor4ub(255, 255, 255, 255);
        glDisable(GL_TEXTURE_2D);
    }
};

}   // Hud
