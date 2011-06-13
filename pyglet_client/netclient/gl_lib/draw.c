#include "./draw.h"

//struct line_vertices
int _draw_line(int r, int g,int b, float x0, float y0, float z0, float x1, float y1, float z1) {

//printf("(%i, %i, %i)\n", r,g,b);

glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);
glBegin(GL_LINES);
glVertex3f(x0,y0,z0); // origin of the line
glVertex3f(x1,y1,z1); // ending point of the line
glEnd();
return 0;
}

int _draw_point(int r, int g,int b, float x0, float y0, float z0) {
glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b);
glBegin(GL_POINTS);
glVertex3f(x0,y0,z0); // point
glEnd();
return 0;
}
