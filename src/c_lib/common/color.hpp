#pragma once

struct Color
{
    union
    {
        unsigned char c[3];
        struct
        {
            unsigned char r,g,b;
        };
    };
};

struct Color4
{
    union
    {
        unsigned char c[4];
        struct
        {
            unsigned char r,g,b,a;
        };
    };
};

inline struct Color interpolate_color(const struct Color a, const struct Color b, float t)
{
    struct Color c;
    c.r = a.r + ((float)(b.r - a.r)) * t;
    c.g = a.g + ((float)(b.g - a.g)) * t;
    c.b = a.b + ((float)(b.b - a.b)) * t;
    return c;
}

inline void print_color(struct Color color)
{
    printf("%d,%d,%d\n", color.r, color.g, color.b);
}

inline struct Color4 color_init(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    struct Color4 color = {{{r,g,b,a}}};
    return color;
}

inline struct Color color_init(unsigned char r, unsigned char g, unsigned char b)
{
    struct Color color = {{{r,g,b}}};
    return color;
} 

inline struct Color color_init(struct Color4 color)
{
    return color_init(color.r, color.g, color.b);
}

inline struct Color4 color_init(struct Color color)
{
    return color_init(color.r, color.g, color.g, 0xFF);
}

inline bool colors_equal(struct Color a, struct Color b)
{
    return (a.r == b.r && a.g == b.g && a.b == b.b);
}

inline bool colors_equal(struct Color4 a, struct Color4 b)
{
    return (a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a);
}

const struct Color4 COLOR_RED = color_init(255,0,0,255);
const struct Color4 COLOR_GREEN = color_init(0,255,0,255);
const struct Color4 COLOR_BLUE = color_init(0,0,255,255);
const struct Color4 COLOR_WHITE = color_init(255,255,255,255);
const struct Color4 COLOR_BLACK = color_init(0,0,0,255);
