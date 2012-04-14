#pragma once

bool cube_intersects(
    float x, float y, float z, float w, float h, float d,
    float x2, float y2, float z2, float w2, float h2, float d2
);

bool rect_intersects(
    float x, float y, float w, float h,
    float x2, float y2, float w2, float h2
);

bool point_in_rect(float px, float py, float rx, float ry, float w, float h);

float distancef(float x, float y, float z, float a, float b, float c);
float distancef_squared(float x, float y, float z, float a, float b, float c);
void rotate_point(float x, float y, float theta, float *x1, float *y1);
float gaussian_value(const float mean, const float stddev, const float x);

const float kE = 2.71828f;
