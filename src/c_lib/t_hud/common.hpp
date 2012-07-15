#pragma once

void u_dot(float x, float y)
{
    /*
        Draw dot in upper left corner
    */
	GL_ASSERT(GL_TEXTURE_2D, false);

    glColor4ub(255, 0, 0, 255);

    float p = 1.0f;
    glBegin(GL_QUADS);

    glVertex2f(x-p,y+p);
    glVertex2f(x+p, y+p);
    glVertex2f(x+p, y-p);
    glVertex2f(x-p, y-p);

    glEnd();
}
