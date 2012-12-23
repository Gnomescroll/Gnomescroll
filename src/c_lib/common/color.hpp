#pragma once

class Color
{
    public:
        union
        {
            unsigned char c[4];
            struct
            {
                unsigned char r,g,b,a;
            };
        };

    Color() :
        r(0), g(0), b(0), a(0xFF)
    {}

    Color(unsigned char r, unsigned char g, unsigned char b) :
        r(r), g(g), b(b), a(0xFF)
    {}

    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) :
        r(r), g(g), b(b), a(a)
    {}
};

static unsigned char _gamma_correction[256];
void init_color_data()
{
    static const float gamma_correction = 2.2f;
    for(int i=0; i<256; i++)
    {
        float intensity = powf(float(i)/256.0f, gamma_correction)*256.0f;
        _gamma_correction[i] = (unsigned char)intensity;
    }
}

inline Color interpolate_color(const Color a, const Color b, float t)
{
    Color c;
    c.r = a.r + float(b.r - a.r) * t;
    c.g = a.g + float(b.g - a.g) * t;
    c.b = a.b + float(b.b - a.b) * t;
    c.a = a.a + float(b.a - a.a) * t;
    return c;
}

inline void print_color(Color color)
{
    printf("%d,%d,%d,%d\n", color.r, color.g, color.b, color.a);
}

inline bool colors_equal(Color a, Color b)
{
    return (a.r == b.r && a.g == b.g && a.b == b.b);
}

inline bool colors_equal_alpha(Color a, Color b)
{
    return (a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a);
}

const Color COLOR_RED = Color(0xFF,0,0,0xFF);
const Color COLOR_GREEN = Color(0,0xFF,0,0xFF);
const Color COLOR_BLUE = Color(0,0,0xFF,0xFF);
const Color COLOR_WHITE = Color(0xFF,0xFF,0xFF,0xFF);
const Color COLOR_BLACK = Color(0,0,0,0xFF);

//convert to linear scale

inline Color color_linearize(Color color)
{
    Color c;
    for (int i=0; i<3; i++)
        c.c[i] = _gamma_correction[color.c[i]];
    return c;
}
