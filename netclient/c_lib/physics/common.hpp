#pragma once

bool cube_intersects(
    float x, float y, float z, float w, float h, float d,
    float x2, float y2, float z2, float w2, float h2, float d2
);

float distancef(float x, float y, float z, float a, float b, float c);

float gaussian_value(const float mean, const float stddev, const float x);

const float kPI = 3.14159f;
const float kE = 2.71828f;
