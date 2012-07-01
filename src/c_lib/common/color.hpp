#pragma once

struct Color {
    unsigned char r,g,b;
};

struct Color4 {
    unsigned char r,g,b,a;
};

struct Color interpolate_color(const struct Color a, const struct Color b, float t)
{
	struct Color c;
	c.r = a.r + ((float)(b.r - a.r)) * t;
	c.g = a.g + ((float)(b.g - a.g)) * t;
	c.b = a.b + ((float)(b.b - a.b)) * t;
	return c;
}

void print_color(struct Color color)
{
	printf("%d,%d,%d\n", color.r, color.g, color.b);
}
