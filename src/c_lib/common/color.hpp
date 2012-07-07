#pragma once

struct Color {
    unsigned char r,g,b;
};

struct Color4 {
    unsigned char r,g,b,a;
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

inline struct Color4 color4_init(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    struct Color4 color = {r,g,b,a};
    return color;
}

inline struct Color color_init(unsigned char r, unsigned char g, unsigned char b)
{
    struct Color color = {r,g,b};
    return color;
} 
